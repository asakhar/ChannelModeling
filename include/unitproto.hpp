#ifndef UNITPROTO_HPP
#define UNITPROTO_HPP

#include "channel.hpp"
#include "metainfo.hxx"
#include <initializer_list>
#include <random>
#include <type_traits>
#include <vector>
/**
 * @brief Prototype for designing Units.
 * You only have to inherit from this class and override *void run()* method
 * Fields *input*, *output* and *meta* are there to be used as data transfer
 * interfaces
 *
 * @tparam In_t input data type
 * @tparam Out_t output data type
 */
template <typename In_t, typename Out_t> struct UnitProto {
public:
  /**
   * @brief Input data to manage from *void run()*
   *
   */
  In_t input;
  /**
   * @brief Output data to be stored from *void run()*
   *
   */
  Out_t output;
  /**
   * @brief Additional user information bus
   *
   */
  MetaInfo meta;
  /**
   * @brief Mersene twister
   *
   */
  std::mt19937 gen;
  /**
   * @brief Processing method to be overloaded
   *
   */
  UnitProto() : gen{std::random_device{}()} {}
  UnitProto(uint32_t seed) : gen{seed} {}

  std::mt19937 &twister() {
    return meta.find<std::mt19937>() == meta.end() ? gen
                                                   : meta.get<std::mt19937>();
  }

  virtual void run() { meta.put(UnitInfo{"Unit prototype has been used."}); }

  virtual ~UnitProto() = default;
  struct UnitInfo {
    char const *info;
  };

  Out_t operator()(In_t &&data, MetaInfo &info) {
    input = std::move(data);
    meta = std::move(info);
    output = Out_t{};
    // output.clear();
    run();
    info = std::move(meta);
    return output;
  }
};
#endif // UNITPROTO_HPP