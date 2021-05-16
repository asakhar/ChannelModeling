#ifndef ERRORCOUNTER_HPP
#define ERRORCOUNTER_HPP

#include "unitproto.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
template <typename InitialData, typename BER>
class BitErrorRate : public UnitProto<std::vector<float>, std::vector<float>> {
public:
  void run() {
    output = std::move(input);
    auto const &init_data = meta.get<InitialData>();
    if (output.size() != init_data.data.size())
      throw std::runtime_error("Incorrect length");
    auto counter = 0UL;
    for (auto i = 0UL; i < output.size(); i++) {
      if (output[i] != init_data.data[i])
        counter++;
    }
    meta.put(BER{counter});
  }
};
template <typename BER>
class BERPrinter : public UnitProto<std::vector<float>, std::vector<float>> {
  std::ostream &m_str;

public:
  BERPrinter(std::ostream &stream = std::cout) : m_str{stream} {}
  void run() {
    output = std::move(input);
    m_str << meta.get<BER>() << std::endl;
  }
};
template <typename BER>
class BER2Var : public UnitProto<std::vector<float>, std::vector<float>> {
  BER &m_ber;

public:
  BER2Var(BER &ber) : m_ber{ber} {}
  void run() {
    output = std::move(input);
    m_ber = meta.get<BER>();
  }
};

#endif // ERRORCOUNTER_HPP
