#ifndef MATRIX_HXX
#define MATRIX_HXX

#include <vector>

template <typename Ty> struct Matrix {
  size_t dimr, dimc;
  std::vector<double> data;
  Matrix() = default;
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

#endif // MATRIX_HXX
