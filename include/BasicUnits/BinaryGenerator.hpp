#ifndef BINARYGENERATOR_HPP
#define BINARYGENERATOR_HPP

#include "unitproto.hpp"
#include <random>
/**
 * @brief Example of data generator without input
 *
 */
class BinaryGenerator : public UnitProto<EmptyObject, std::vector<bool>> {
public:
  /**
   * @brief Example of wrapper for meta info containing singleton object
   *
   */
  struct GeneratorOut {
    std::vector<bool> data;
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
      : p{probability}, n{count}, gen{seed}, bern{p} {}
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
      bool result = bern(gen);
      item = result;
      out.data.emplace_back(result);
    }
    meta.put(out);
  }
  double p;
  size_t n;
  std::mt19937 gen;
  std::bernoulli_distribution bern{0};
};

#endif // BINARYGENERATOR_HPP