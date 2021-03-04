#ifndef UNIFORMDOUBLEDISTR_HXX
#define UNIFORMDOUBLEDISTR_HXX
#include <random>

class UniformDoubleDistr {
public:
  UniformDoubleDistr() = default;
  UniformDoubleDistr(double from, double to)
      : gen{rd()}, dist{from, std::nextafter(
                                  to, std::numeric_limits<double>::max())} {}
  UniformDoubleDistr(UniformDoubleDistr const &ot)
      : gen{ot.gen}, dist{ot.dist} {};
  inline double operator()() { return dist(gen); }

private:
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
};

#endif // UNIFORMDOUBLEDISTR_HXX
