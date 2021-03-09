#ifndef REPETITIONCODER_HPP
#define REPETITIONCODER_HPP

#include "unitproto.hpp"
#include <vector>

class RepetitionEncoder
    : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  RepetitionEncoder(size_t repeats) : m_repeats{repeats} {}
  void run() override {
    output.reserve(input.size() * m_repeats);
    for (auto item : input)
      for (size_t i = 0; i < m_repeats; ++i)
        output.push_back(item);
  }
  size_t m_repeats;
};

class RepetitionDecoder
    : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  RepetitionDecoder(size_t repeats) : m_repeats{repeats} {}
  void run() override {
    output.reserve(input.size() / m_repeats);
    for (size_t i = 0; i < output.capacity(); i++) {
      size_t sum = 0;
      for (size_t j = 0; j < m_repeats; j++)
        sum += (size_t)input[i * m_repeats + j];
      output.push_back(sum > m_repeats / 2);
    }
  }
  size_t m_repeats;
};

#endif // REPETITIONCODER_HPP
