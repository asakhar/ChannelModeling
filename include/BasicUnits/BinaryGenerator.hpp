#ifndef BINARYGENERATOR_HPP
#define BINARYGENERATOR_HPP

#include "unitproto.hpp"
#include <bits/stdint-uintn.h>
#include <random>
/**
 * @brief Example of data generator without input
 *
 */
class BinaryGenerator : public UnitProto<EmptyObject, std::vector<uint8_t>> {
public:
  /**
   * @brief Example of wrapper for meta info containing singleton object
   *
   */
  struct GeneratorOut {
    std::vector<uint8_t> data;
  };

  BinaryGenerator() = default;
  /**
   * @brief Construct a new Binary Generator object
   *
   * @param probability probability of generating 1
   * @param count number of bits to generate in one run
   * @param seed
   */
  BinaryGenerator(double probability, size_t count,
                  uint32_t seed = std::random_device{}())
      : UnitProto{seed}, p{probability}, n{count}, bern{p} {}
  /**
   * @brief Processing method.
   * As shown it use no input at all.
   * And put some meta info to have an access to them from other units or from
   * outsize of model
   *
   */
  void run() override {
    GeneratorOut out;
    output.resize(n);
    for (auto item : output) {
      uint8_t result = static_cast<uint8_t>(bern(twister()));
      item = result;
      out.data.emplace_back(result);
    }
    meta.put(out);
  }
  double p;
  size_t n;
  std::bernoulli_distribution bern{0};
};

#endif // BINARYGENERATOR_HPP