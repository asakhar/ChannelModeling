#ifndef BINARYSYMMETRICCHANNEL_HXX
#define BINARYSYMMETRICCHANNEL_HXX

#include "unitproto.hpp"
#include "Utility/UniformDoubleDistr.hxx"
#include <vector>

class BinarySymmetricChannel : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  BinarySymmetricChannel() = default;
  BinarySymmetricChannel(double bitflip_chance) : p{bitflip_chance}, dist{0., 1.} {}
  void run() override {
    output = std::move(input);
    for(auto item: output) 
      item = item ^ (dist() <= p);
  }
private:
  double p = 0;
  UniformDoubleDistr dist{.0, .0};
};

#endif // BINARYSYMMETRICCHANNEL_HXX
