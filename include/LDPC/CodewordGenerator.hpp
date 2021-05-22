#ifndef CODEWORDGENERATOR_HPP
#define CODEWORDGENERATOR_HPP
#include "src/matrix_generate.h"

#include "Decoder.hpp"
#include "channel.hpp"
#include "unitproto.hpp"
class CodewordGenerator : public UnitProto<EmptyObject, std::vector<float>> {
  DecoderMinSumByIndex &decoder;

public:
  CodewordGenerator(DecoderMinSumByIndex &decoder_instance)
      : decoder{decoder_instance}
  {}
  void run() override {
    output.resize(decoder.n);
    codeword_generate(decoder.row_num, decoder.col_num, output,
                      decoder.check_matrix);
  }
};

#endif // CODEWORDGENERATOR_HPP
