#include "BasicUnits/BinaryGenerator.hpp"
#include "BasicUnits/MarkovBinaryChannel.hpp"
#include "BasicUnits/RepetitionCoder.hpp"
#include "metainfo.hxx"
#include <array>
#include <fstream>

int main(int /*argc*/, char const * /*argv*/[]) {
  double const pi0 = 0;
  double const pi1 = 0.3;
  double const pi2 = 0.7;

  double const p00 = 7.;
  double const p01 = 3.;
  double const p02 = 0.;
  double const p10 = 7.;
  double const p11 = 2.;
  double const p12 = 1.;
  double const p20 = 6.;
  double const p21 = 1.;
  double const p22 = 3.;

  int const n = 100;

  Matrix<double> P{std::vector<std::vector<double>>{
      {p00, p01, p02}, {p10, p11, p12}, {p20, p21, p22}}};
  P.normalize(P.Rows);
  std::cout << "P_state_transition =\n";
  for (auto row : P) {
    std::cout << "[";
    for (auto &col : row)
      std::cout << std::setw(4) << col;
    std::cout << " ]\n";
  }
  double const prob = 0.3;
  int const seed = 12;
  int const repetition_length = 5;
  Model model{BinaryGenerator(prob, n, seed),
              RepetitionEncoder{repetition_length},
              MarkovBinaryChannel(3, 0, P, {pi0, pi1, pi2}, seed),
              RepetitionDecoder{repetition_length}};
  auto const iterations = 10000;
  std::array<size_t, n + 1> exact_errs{0};
  for (int i = 0; i < iterations; i++) {
    MetaInfo meta;
    auto res = model(meta);
    auto exact_out = meta.get<BinaryGenerator::GeneratorOut>();
    auto zip = [](auto &v1, auto &v2)
        -> std::vector<std::pair<decltype(v1[0]), decltype(v2[0])>> {
      std::vector<std::pair<decltype(v1[0]), decltype(v2[0])>> result;
      for (size_t i = 0; i < std::min(v1.size(), v2.size()); ++i) {
        result.emplace_back(v1[i], v2[i]);
      }
      return std::move(result);
    };
    int errs = 0;
    for (auto &pair : zip(res, exact_out.data))
      errs += static_cast<int>((bool)pair.first ^ (bool)pair.second);
    ++exact_errs[errs];
  }
  std::ofstream file{"output.csv"};
  std::cout << "total iterations: " << iterations << "\nP(m,n)  as  n=" << n
            << ":";
  std::cout.precision(std::numeric_limits<double>::max_digits10);
  file.precision(std::numeric_limits<double>::max_digits10);
  file << "number_of_errors,probability\n";
  for (size_t i = 0; auto &item : exact_errs) {
    double probability = (double)item / iterations;
    file << i << "," << std::fixed << probability << "\n";
    std::cout << "\n P(" << std::setw(2) << i++ << "," << n
              << ") = " << std::fixed << probability;
  }
  std::cout << std::endl;
  file.close();
  return 0;
}
