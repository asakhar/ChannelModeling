#ifndef CODEWORDGENERATOR_HPP
#define CODEWORDGENERATOR_HPP
#include "src/matrix_generate.h"

#include "channel.hpp"
#include "unitproto.hpp"
class CodewordGenerator : public UnitProto<EmptyObject, std::vector<float>> {
  int m_n;
  int m_k;
  int m_l;
  std::vector<int> m_check_mat;

public:
  CodewordGenerator(int n, int k, int l, std::vector<int> const &check_matrix)
      : m_n{n}, m_k{k}, m_check_mat{check_matrix}, m_l{l} {}
  void run() override {
    output.resize(m_k * m_l);
    codeword_generate(m_n * m_l, m_k * m_l, output, m_check_mat);
  }
};

#endif // CODEWORDGENERATOR_HPP
