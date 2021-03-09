#ifndef BINARYSYMMETRICCHANNEL_HXX
#define BINARYSYMMETRICCHANNEL_HXX

#include "unitproto.hpp"
#include <random>
#include <vector>

class BinarySymmetricChannel : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  BinarySymmetricChannel() = default;
  BinarySymmetricChannel(double bitflip_chance, uint32_t seed = std::random_device{}()) : p{bitflip_chance}, gen{seed}, dist{0., 1.} {}
  void run() override {
    output = std::move(input);
    for(auto item: output) 
      item = item ^ (dist(gen) <= p);
  }
private:
  double p = 0;
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
};

#endif // BINARYSYMMETRICCHANNEL_HXX
