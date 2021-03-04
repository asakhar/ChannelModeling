#ifndef MARKOVBINARYCHANNEL_HXX
#define MARKOVBINARYCHANNEL_HXX

#include "Utility/Matrix.hxx"
#include "Utility/UniformDoubleDistr.hxx"
#include "unitproto.hpp"
#include <cmath>
#include <iomanip>
#include <limits>
#include <random>
#include <vector>

class MarkovBinaryChannel : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  MarkovBinaryChannel() = default;
  MarkovBinaryChannel(size_t number_of_states, size_t initial_state,
                      Matrix<double> transition_probabilities,
                      std::vector<double> error_probabilities)
      : nstates{number_of_states}, initstate{initial_state},
        P{std::move(transition_probabilities)},
        Pis{std::move(error_probabilities)}, dist{0., 1.} {
    P.normalize(P.Rows);
  }

  void run() override {
    auto state = initstate;
    output = std::move(input);
    for (auto bit : output) {
      auto bitflip = dist();
      bit = bit ^ (bitflip < Pis[state]);

      auto translation = dist();
      auto i = 0UL;
      for (; i < nstates - 1; i++)
        if (translation <= P[state][i])
          break;
      state = i;
    }
  }

  size_t nstates = 0UL, initstate = 0UL;
  Matrix<double> P{};
  std::vector<double> Pis{};
  UniformDoubleDistr dist{.0, .0};
};

#endif // MARKOVBINARYCHANNEL_HXX