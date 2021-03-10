#ifndef MATRIX_HXX
#define MATRIX_HXX

#include <vector>
/**
 * @brief Helper Matrix struct for Markov Chain generator
 *
 * @tparam Ty
 */
template <typename Ty> struct Matrix {
  /**
   * @brief Dimentions
   *
   */
  size_t dimr, dimc;
  /**
   * @brief Containing data
   *
   */
  std::vector<double> data;
  Matrix() = default;
  /**
   * @brief Construct a new Matrix object from 2-dim vector
   *
   * @param rows
   */
  Matrix(std::vector<std::vector<Ty>> const &rows) {
    dimr = rows.size();
    dimc = rows.at(dimr - 1).size();
    data.reserve(dimr * dimc);
    for (auto &row : rows)
      for (auto &col : row)
        data.emplace_back(col);
  }
  /**
   * @brief Helper struct for wrapping Matrix rows
   *
   */
  struct RowWrapper {
    /**
     * @brief Get emelent reference by index in row
     *
     * @param y index
     * @return Ty&
     */
    Ty &operator[](size_t y) { return ref.data[row * ref.dimc + y]; }
    /**
     * @brief Construct a new Row Wrapper object
     *
     * @param mref
     * @param mrow
     */
    RowWrapper(Matrix<Ty> &mref, size_t mrow) : row{mrow}, ref{mref} {}
    /**
     * @brief begin iterator
     *
     * @return Ty*
     */
    Ty *begin() { return &ref.data[row * ref.dimc]; }
    /**
     * @brief end iterator
     *
     * @return Ty*
     */
    Ty *end() { return &ref.data[(row + 1) * ref.dimc]; }
    /**
     * @brief Row comparison
     *
     * @param other
     * @return true
     * @return false
     */
    bool operator!=(RowWrapper const &other) const {
      return &ref != &other.ref || row != other.row;
    }
    /**
     * @brief move to the next row (iterator-like behaviour)
     *
     * @return RowWrapper&
     */
    RowWrapper &operator++() {
      ++row;
      return *this;
    }
    /**
     * @brief iterator-like behaviour for using in range-based for loops
     *
     * @return RowWrapper&
     */
    RowWrapper &operator*() { return *this; }
    /**
     * @brief Row index
     *
     */
    size_t row;
    /**
     * @brief Matrix reference
     *
     */
    Matrix<Ty> &ref;
  };
  /**
   * @brief Get row by index
   *
   * @param x number of row
   * @return RowWrapper
   */
  RowWrapper operator[](size_t x) { return {*this, x}; }
  /**
   * @brief begin iterator
   *
   * @return RowWrapper
   */
  RowWrapper begin() { return {*this, 0}; }
  /**
   * @brief end iterator
   *
   * @return RowWrapper
   */
  RowWrapper end() { return {*this, dimr}; }
  /**
   * @brief helper enum
   *
   */
  enum Dims : int { Rows = 1, Columns = 2 };
  /**
   * @brief change matrix such that the following dimensions' sums become equal
   * to 1
   *
   * @param dims
   */
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
