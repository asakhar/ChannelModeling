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
      : p{probability}, n{count}, gen{seed}, bern{p} {}
  /**
   * @brief Processing method.
   * As shown it uses no input at all.
   *
   */
  void run() override {
    output.resize(n);
    for (auto &item : output) {
      item = static_cast<uint8_t>(bern(gen));
    }
  }
  double p;
  size_t n;
  std::mt19937 gen;
  std::bernoulli_distribution bern;
};

#endif // BINARYGENERATOR_HPP