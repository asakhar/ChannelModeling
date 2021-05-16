#ifndef REPETITIONCODER_HPP
#define REPETITIONCODER_HPP

#include "unitproto.hpp"
#include <bits/stdint-uintn.h>
#include <vector>
/**
 * @brief Example of repetition encoder unit
 *
 */
class RepetitionEncoder
    : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
public:
  /**
   * @brief Construct a new Repetition Encoder object
   *
   * @param repeats how many times to repeat symbols
   */
  RepetitionEncoder(size_t repeats) : m_repeats{repeats} {}
  /**
   * @brief Processing method. Puts result to output field
   *
   */
  void run() override {
    output.reserve(input.size() * m_repeats);
    for (auto const&item : input)
      for (size_t i = 0; i < m_repeats; ++i)
        output.push_back(item);
  }
  size_t m_repeats;
};
/**
 * @brief Example of repetition decoder unit
 *
 */
class RepetitionDecoder
    : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
public:
  /**
   * @brief Construct a new Repetition Decoder object
   *
   * @param repeats number of repeating symbols in input (should be the same as
   * in encode in order to work properly)
   */
  RepetitionDecoder(size_t repeats) : m_repeats{repeats} {}
  /**
   * @brief Processing method
   *
   */
  void run() override {
    output.reserve(input.size() / m_repeats);
    for (size_t i = 0; i < output.capacity(); i++) {
      size_t sum = 0;
      for (size_t j = 0; j < m_repeats; j++)
        sum += (size_t)input[i * m_repeats + j];
      output.push_back(static_cast<uint8_t>(sum > m_repeats / 2));
    }
  }
  size_t m_repeats;
};

#endif // REPETITIONCODER_HPP
