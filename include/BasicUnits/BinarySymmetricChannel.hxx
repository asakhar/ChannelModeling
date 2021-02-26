#ifndef BINARYSYMMETRICCHANNEL_HXX
#define BINARYSYMMETRICCHANNEL_HXX

#include "unitproto.hxx"
#include "Utility/UniformDoubleDistr.hxx"

class BinarySymmetricChannel : public UnitProto<bool, bool> {
public:
  BinarySymmetricChannel(double bitflip_chance) : p{bitflip_chance}, dist{0., 1.} {}
  void run() override {
    output = std::move(input);
    for(auto item: output) 
      item = item ^ (dist() <= p);
  }
private:
  double p;
  UniformDoubleDistr dist;
};

#endif // BINARYSYMMETRICCHANNEL_HXX
