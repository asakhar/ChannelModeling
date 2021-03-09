#ifndef BINARYGENERATOR_HPP
#define BINARYGENERATOR_HPP

#include "unitproto.hpp"
#include <random>

class BinaryGenerator : public UnitProto<EmptyObject, std::vector<bool>> {
public:
  struct GeneratorOut {
    std::vector<bool> data;
  };

  BinaryGenerator() = default;
  BinaryGenerator(double probability, size_t count, uint32_t seed = std::random_device{}())
      : p{probability}, n{count}, gen{seed}, bern{p} {}
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