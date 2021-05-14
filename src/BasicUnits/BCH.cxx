#include "BasicUnits/BCH.hpp"

#include <algorithm>
#include <bch_codec.h>

#include <memory>
#include <vector>

BCHEncoder::BCHEncoder(int m, int t, unsigned int prim_poly)
    : m_control{init_bch(m, t, prim_poly)} {}
BCHEncoder::~BCHEncoder() { free_bch(m_control); }
void BCHEncoder::encode(std::vector<uint8_t> &data) {
  data.resize(data.size() + m_control->ecc_bytes);
  std::fill(data.begin() + m_control->ecc_bytes, data.end(), '\0');
  encode_bch(m_control, data.data(), data.size() - m_control->ecc_bytes,
             data.data() + m_control->ecc_bytes);
}
void BCHDecoder::decode(std::vector<uint8_t> &data) {
  m_errloc.resize(m_encoder.m_control->t);
  std::fill(m_errloc.begin(), m_errloc.end(), 0U);
  auto size = decode_bch(m_encoder.m_control, data.data(),
                         data.size() - m_encoder.m_control->ecc_bytes,
                         data.data() + m_encoder.m_control->ecc_bytes, nullptr,
                         nullptr, m_errloc.data());
  m_errloc.resize(size);
}
void BCHDecoder::correct(std::vector<uint8_t> &data) {
  correct_bch(m_encoder.m_control, data.data(),
              data.size() - m_encoder.m_control->ecc_bytes, m_errloc.data(),
              m_errloc.size());
  data.resize(data.size() - m_encoder.m_control->ecc_bytes);
}

BCHDecoder::BCHDecoder(BCHEncoder &encoder) : m_encoder(encoder) {
  m_errloc.reserve(encoder.m_control->t);
}

void BCHEncoder::run() {
  encode(input);
  output = std::move(input);
}

void BCHDecoder::run() {
  decode(input);
  correct(input);
  output = std::move(input);
}

BCHLogger::BCHLogger(BCHEncoder &encoder) : m_encoder{encoder} {}
void BCHLogger::run() {
  ErrorCorrectionCodes md;
  md.ecc.resize(m_encoder.m_control->ecc_bytes);
  std::copy(input.end() - m_encoder.m_control->ecc_bytes - 1, input.end(),
            md.ecc.begin());
  meta.put(std::move(md));
  output = std::move(input);
}