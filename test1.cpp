#include "BasicUnits/DataLogger.hpp"
#include "BasicUnits/ErrorCounter.hpp"
#include "BasicUnits/GaussianChannel.hpp"
#include "BasicUnits/Modulator.hpp"
#include "LDPC/CodewordGenerator.hpp"
#include "LDPC/Decoder.hpp"

#include "channel.hpp"
#include "metainfo.hxx"
#include <bits/stdint-uintn.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>

using json = nlohmann::json;

template <typename T> std::string atos(T x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << R"(Usage

  )" << argv[0]
              << R"( <path-to-cl-dir>
              )";
    return -1;
  }
  std::string cldir = argv[1];
  std::cout << "Current CL path: \n" << cldir << std::endl;

  DecoderMinSumByIndex decoder(
      2, 3, 100, 100, cldir,
      124U); // 123 is the worst seed for generating matrix
  GaussianChannel channel{0.F};
  using DataType = std::vector<float>;
  using Container = struct { DataType data; };
  using BER_t = struct { uint64_t data; };
  BER_t ber;
  Model model(
      CodewordGenerator(decoder),
      DataLogger<DataType, Container>{}, /*
       DataPrinter<DataType, Container>{[](float x) { return itos((int)x); }},*/
      Modulator(), std::move(channel), Demodulator(), decoder,
      BitErrorRate<Container, BER_t>() /*, BERPrinter<BER_t>()*/, BER2Var{ber});

  const auto max_iters = 100;
  const auto max_ber = 100;
  const auto max_ser = 30;
  const auto power_step = .005F;
  const auto max_power = .7F;
  for (int i = 0; i < 5; i++) {
    float noise_power = .4F;
    json data;
    std::ofstream file{"result" + atos(i + 2) + ".json"};
    MetaInfo meta;
    meta.put(std::mt19937{i==1 ? 123U : (123U+i)});
    while (noise_power < max_power) {
      model.get<3>().setNoisePower(noise_power);
      auto iters = 0UL;
      auto ser = 0UL;
      auto ber_cummulative = 0UL;
      while (ber_cummulative < max_ber && ser < max_ser && iters < max_iters) {
        iters++;
        model(meta);
        ber_cummulative += ber.data;
        if (ber.data > 0)
          ser++;

        std::cout << "BER: " << ber.data << "; BERC: " << ber_cummulative
                  << "; SER: " << ser << "; iters: " << iters << std::endl;
      }
      data[atos(noise_power)] = {
          {"BERC", ber_cummulative}, {"SER", ser}, {"iters", iters}};

      std::cout << "Number of iterations until reaching BER or CER limit: "
                << iters << std::endl;

      noise_power += power_step;
    }
    file << data;
    file.close();
  }
}