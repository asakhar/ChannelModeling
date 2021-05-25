#ifndef BCH_HPP
#define BCH_HPP

#include "unitproto.hpp"
#include <cstdint>
#include <vector>
struct bch_control;

class BCHEncoder
    : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
public:
  BCHEncoder(int m, int t, unsigned int prim_poly = 0);
  ~BCHEncoder();
  BCHEncoder(BCHEncoder &&);
  BCHEncoder(BCHEncoder const &);
  BCHEncoder& operator=(BCHEncoder &&);
  BCHEncoder& operator=(BCHEncoder const&);
  void run() override;

private:
  friend class BCHDecoder;
  friend class BCHLogger;
  void encode(std::vector<uint8_t> &data);

  bch_control *m_control;
  unsigned int m_prim_poly;
};

class BCHDecoder
    : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
public:
  BCHDecoder(BCHEncoder &encoder);

  void run() override;

private:
  void decode(std::vector<uint8_t> &data);
  void correct(std::vector<uint8_t> &data);

  std::vector<uint32_t> m_errloc;
  BCHEncoder &m_encoder;
};

class BCHLogger : public UnitProto<std::vector<uint8_t>, std::vector<uint8_t>> {
  struct ErrorCorrectionCodes {
    std::vector<uint8_t> ecc;
  };
  BCHLogger(BCHEncoder &encoder);
  void run() override;

private:
  BCHEncoder &m_encoder;
};

#endif // BCH_HPP
