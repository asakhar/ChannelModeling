#ifndef MODULATOR_HPP
#define MODULATOR_HPP

#include "unitproto.hpp"

class Modulator : public UnitProto<std::vector<float>, std::vector<float>> {
public:
  void run() {
    output = std::move(input);
    for (auto &item : output) {
      item = 1. - 2. * item;
    }
  }
};
class Demodulator : public UnitProto<std::vector<float>, std::vector<float>> {
public:
  void run() {
    output = std::move(input);
    for (auto &item : output) {
      item = item > 0. ? 0. : 1.;
    }
  }
};

#endif // MODULATOR_HPP
