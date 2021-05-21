#ifndef CODEWORDGENERATOR_HPP
#define CODEWORDGENERATOR_HPP
#include "src/matrix_generate.h"

#include "Decoder.hpp"
#include "channel.hpp"
#include "unitproto.hpp"
class CodewordGenerator : public UnitProto<EmptyObject, std::vector<float>> {
  DecoderMinSumByIndex &decoder;
  // std::vector<int> m_check_mat;
  // std::vector<float> cw;

public:
  CodewordGenerator(DecoderMinSumByIndex &decoder_instance)
      : decoder{decoder_instance} // , m_check_mat{decoder.check_matrix}
  {}
  void run() override {
    // if (cw.empty()) {
    output.resize(decoder.n);
    codeword_generate(decoder.row_num, decoder.col_num, output,
                      decoder.check_matrix);
    //   cw = output;
    // } else {
    //   output = cw;
    // }
  }
};

#endif // CODEWORDGENERATOR_HPP
