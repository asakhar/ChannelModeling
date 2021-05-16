#ifndef MARKOVBINARYCHANNEL_HXX
#define MARKOVBINARYCHANNEL_HXX

#include "Utility/Matrix.hxx"
#include "unitproto.hpp"
#include <bits/stdint-uintn.h>
#include <cmath>
#include <iomanip>
#include <limits>
#include <random>
#include <vector>
/**
 * @brief Example of Markov Chain Channel unit
 *
 */
class MarkovBinaryChannel
    : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
public:
  MarkovBinaryChannel() = default;
  /**
   * @brief Construct a new Markov Binary Channel object
   *
   * @param number_of_states
   * @param initial_state
   * @param transition_probabilities
   * @param error_probabilities
   * @param seed
   */
  MarkovBinaryChannel(size_t number_of_states, size_t initial_state,
                      Matrix<double> transition_probabilities,
                      std::vector<double> error_probabilities,
                      uint32_t seed = std::random_device{}())
      : nstates{number_of_states}, initstate{initial_state},
        P{std::move(transition_probabilities)},
        Pis{std::move(error_probabilities)}, gen{seed}, dist{0., 1.} {
    P.normalize(P.Rows);
  }
  /**
   * @brief Processing method
   *
   */
  void run() override {
    auto state = initstate;
    output = std::move(input);
    for (auto &bit : output) {
      auto bitflip = dist(gen);
      bit = bit ^ static_cast<int>(bitflip < Pis[state]);

      auto translation = dist(gen);
      auto i = 0UL;
      for (; i < nstates - 1; i++)
        if (translation <= P[state][i])
          break;
      state = i;
    }
  }

  size_t nstates = 0UL, initstate = 0UL;
  Matrix<double> P;
  std::vector<double> Pis;
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
};

#endif // MARKOVBINARYCHANNEL_HXX