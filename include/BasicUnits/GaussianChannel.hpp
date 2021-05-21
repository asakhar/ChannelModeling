#ifndef GAUSSIANCHANNEL_HPP
#define GAUSSIANCHANNEL_HPP

#include "unitproto.hpp"
#include <bits/stdint-uintn.h>
#include <cmath>
#include <random>
#include <vector>

class GaussianChannel
    : public UnitProto<std::vector<float>, std::vector<float>> {
  std::mt19937 m_mersene;
  std::normal_distribution<float> m_nd;

public:
  GaussianChannel(float noise_power, uint32_t seed = std::random_device{}())
      : m_mersene{seed}, m_nd{
                             0,
                             static_cast<float>(std::sqrt(noise_power / 2.))} {
  }
  void setNoisePower(float noise_power) {
    auto std = m_nd.stddev();
    m_nd = std::normal_distribution<float>{
        0, static_cast<float>(std::sqrt(noise_power / 2.))};
    auto std2 = m_nd.stddev();
  }
  void run() {
    output = std::move(input);
    for (auto &item : output) {
      item += m_nd(m_mersene);
    }
  }
};

#endif // GAUSSIANCHANNEL_HPP
