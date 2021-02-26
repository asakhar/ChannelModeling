#include "unitproto.hxx"
// #include <matrix>
#include <cmath>
#include <iomanip>
#include <limits>
#include <random>
#include <vector>

template <typename Ty> struct Matrix {
  size_t dimr, dimc;
  std::vector<double> data;
  Matrix(std::vector<std::vector<Ty>> const &rows) {
    dimr = rows.size();
    dimc = rows.at(dimr - 1).size();
    data.reserve(dimr * dimc);
    for (auto &row : rows)
      for (auto &col : row)
        data.emplace_back(col);
  }
  struct RowWrapper {
    Ty &operator[](size_t y) { return ref.data[row * ref.dimc + y]; }
    RowWrapper(Matrix<Ty> &mref, size_t mrow) : row{mrow}, ref{mref} {}
    Ty *begin() { return &ref.data[row * ref.dimc]; }
    Ty *end() { return &ref.data[(row + 1) * ref.dimc]; }
    bool operator!=(RowWrapper const &other) const {
      return &ref != &other.ref || row != other.row;
    }
    RowWrapper &operator++() {
      ++row;
      return *this;
    }
    RowWrapper &operator*() { return *this; }
    size_t row;
    Matrix<Ty> &ref;
  };
  RowWrapper operator[](size_t x) { return {*this, x}; }
  RowWrapper begin() { return {*this, 0}; }
  RowWrapper end() { return {*this, dimr}; }
  enum Dims : int { Rows = 1, Columns = 2 };
  void normalize(Dims dims = (Dims)(Rows | Columns)) {
    Ty sum = 0;
    if (dims == (Rows | Columns)) {
      for (auto row : *this)
        for (auto &col : row)
          sum += std::abs(col);
      for (auto row : *this)
        for (auto &col : row)
          col /= sum;
    } else if (dims == Rows) {
      for (auto row : *this) {
        sum = 0;
        for (auto &col : row)
          sum += std::abs(col);
        for (auto &col : row)
          col /= sum;
      }
    } else if (dims == Columns) {
      std::vector<Ty> sums;
      sums.resize(dimc, 0);
      for (auto row : *this)
        for (auto &col : row)
          sums[&col - row.begin()] += std::abs(col);
      for (auto row : *this)
        for (auto &col : row)
          col /= sums[&col - row.begin()];
    }
  }
};

class MarkovChannel : public UnitProto<bool, bool> {
public:
  MarkovChannel(size_t number_of_states, size_t initial_state,
                Matrix<double> transition_probabilities,
                std::vector<double> error_probabilities)
      : nstates{number_of_states}, initstate{initial_state},
        P{std::move(transition_probabilities)}, Pis{std::move(
                                                    error_probabilities)} {
    P.normalize(P.Rows);
  }
  std::pair<std::vector<bool>, MetaInfo> operator()(std::vector<bool> &&data,
                                                    MetaInfo &&info) override {
    // for (auto r : P) {
    //   for (auto &c : r)
    //     std::cout << std::setw(7) << std::setprecision(4) << c;
    //   std::cout << "\n";
    // }
    // ###########
    auto state = initstate;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist{
        0., std::nextafter(1., std::numeric_limits<double>::max())};

    for (auto bit : data) {
      auto bitflip = dist(gen);
      bit = bit ^ (bitflip < Pis[state]);

      auto translation = dist(gen);
      auto i = 0;
      for (; i < nstates - 1; i++)
        if (translation <= P[state][i])
          break;
      state = i;
      // std::cout << state << " ";
    }
    // std::cout << "\n";
    return std::pair{std::move(data), std::move(info)};
  }

  size_t nstates, initstate;
  Matrix<double> P;
  std::vector<double> Pis;
};

int main(int argc, char const *argv[]) {
  double const pi0 = 0;
  double const pi1 = 0.3;
  double const pi2 = 0.7;

  double const p00 = 0.7;
  double const p01 = 0.3;
  double const p02 = 0.0;
  double const p10 = 0.7;
  double const p11 = 0.2;
  double const p12 = 0.1;
  double const p20 = 0.6;
  double const p21 = 0.1;
  double const p22 = 0.3;

  std::vector<bool> input;
  int const n = 46;
  input.resize(n, false);
  Model<bool, bool> model;

  Matrix<double> P{std::vector<std::vector<double>>{
      {p00, p01, p02}, {p10, p11, p12}, {p20, p21, p22}}};
  std::cout << "P_state_transition =\n";
  for(auto row: P)
  {
    std::cout << "[";
    for(auto& col : row)
      std::cout << std::setw(4) << col;
    std::cout << " ]\n";
  }
  model >> MarkovChannel(3, 0, P, {pi0, pi1, pi2});
  auto const iterations = 100000;
  // std::vector<std::vector<bool>> exact1_2or3;
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
    // if (errs == 1 || errs == 2 || errs == 3) {
    //   // exact1_2or3.emplace_back(std::move(res));
    //   for (auto item : res)
    //     std::cout << item << " ";
    //   std::cout << std::endl;
    // }
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
