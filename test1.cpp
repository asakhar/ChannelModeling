#include "BasicUnits/DataLogger.hpp"
#include "BasicUnits/ErrorCounter.hpp"
#include "BasicUnits/GaussianChannel.hpp"
#include "BasicUnits/Modulator.hpp"
#include "LDPC/CodewordGenerator.hpp"
#include "LDPC/Decoder.hpp"

#include "channel.hpp"
#include <bits/stdint-uintn.h>
#include <sstream>
std::string itos(int x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

int main() {
  DecoderMinSumByIndex decoder(2, 3, 10);
  using DataType = std::vector<float>;
  using Container = struct { DataType data; };
  using BER_t = struct { uint64_t data; };
  BER_t ber;
  Model model(
      CodewordGenerator(2, 3, 10, decoder.check_matrix),
      DataLogger<DataType, Container>{}, /*
       DataPrinter<DataType, Container>{[](float x) { return itos((int)x); }},*/
      Modulator(), GaussianChannel(0.1), Demodulator(), decoder,
      BitErrorRate<Container, BER_t>() /*, BERPrinter<BER_t>()*/, BER2Var{ber});

  const auto max_iters = 1000;
  const auto max_ber = 10;
  if (max_ber) {
    auto iters = 0ul;
    while (ber.data < max_ber) {
      iters++;
      model();
      std::cout << ber.data << std::endl;
    }
    std::cout << "Number of iterations until reaching BER limit" << iters
              << std::endl;
  } else {

    for (auto iters = 0ul; iters < max_iters; iters++) {
      model();
      std::cout << ber.data << std::endl;
    }
  }

  //   std::vector<float> codeword(30);
  //   codeword_generate(20, 30, codeword, decoder.check_matrix);
  //   for (auto item : codeword)
  //     std::cout << item << ' ';
  //   std::cout << std::endl;
  //   mistake_generate(codeword);
  //   for (auto item : codeword)
  //     std::cout << item << ' ';
  //   std::cout << std::endl;
  //   auto ret = model(std::move(codeword));
  //   for (auto item : ret)
  //     std::cout << item << ' ';
  // std::cout << std::endl;
}