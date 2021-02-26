#ifndef BINARYGENERATOR_HXX
#define BINARYGENERATOR_HXX

#include "unitproto.hxx"
#include <random>

class BinaryGenerator : public UnitProto<BasicUnit::EmptyObject, bool> {
public:
  struct GeneratorOut {
    std::vector<bool> data;
  };
  BinaryGenerator(double probability, size_t count)
      : p{probability}, n{count}, gen{std::random_device{}()}, bern{p} {}
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
  std::bernoulli_distribution bern;
};

#endif // BINARYGENERATOR_HXX