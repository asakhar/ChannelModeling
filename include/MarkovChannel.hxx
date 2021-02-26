#include "unitproto.hxx"
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
    }
    return std::pair{std::move(data), std::move(info)};
  }

  size_t nstates, initstate;
  Matrix<double> P;
  std::vector<double> Pis;
};