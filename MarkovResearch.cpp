#include "BasicUnits/MarkovBinaryChannel.hxx"

int main(int /*argc*/, char const */*argv*/[]) {
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

  std::vector<bool> input;
  int const n = 46;
  input.resize(n, false);
  Model<bool, bool> model;

  Matrix<double> P{std::vector<std::vector<double>>{
      {p00, p01, p02}, {p10, p11, p12}, {p20, p21, p22}}};
  P.normalize(P.Rows);
  std::cout << "P_state_transition =\n";
  for(auto row: P)
  {
    std::cout << "[";
    for(auto& col : row)
      std::cout << std::setw(4) <<  col;
    std::cout << " ]\n";
  }
  model >> MarkovBinaryChannel(3, 0, P, {pi0, pi1, pi2});
  auto const iterations = 1000000;
  auto exact1 = 0;
  auto exact2 = 0;
  auto exact3 = 0;
  for (int i = 0; i < iterations; i++) {
    auto res = model(input);
    int errs = 0;
    for (auto bit : res)
      errs += static_cast<int>(bit);
    switch (errs) {
    case 1:
      exact1++;
      break;
    case 2:
      exact2++;
      break;
    case 3:
      exact3++;
      break;
    default:
      break;
    }
  }
  std::cout << "total iterations: " << iterations << "\nP(m,n)  as  n=46:"
               "\n P(1,46) = "
            << (double)exact1 / iterations
            << "\n P(2,46) = " << (double)exact2 / iterations
            << "\n P(3,46) = " << (double)exact3 / iterations << std::endl;
  // for (auto &cur : exact1_2or3) {
  //   for (auto item : cur)
  //     std::cout << item << " ";
  //   std::cout << std::endl;
  // }
  // for (auto item : res) {
  //   std::cout << item << " ";
  // }
  // std::cout << std::endl;
  return 0;
}
