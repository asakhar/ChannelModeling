#ifndef UNITPROTO_HPP
#define UNITPROTO_HPP

#include "channel.hpp"
#include "metainfo.hxx"
#include <initializer_list>
#include <random>
#include <type_traits>
#include <vector>

template <typename In_t, typename Out_t> class UnitProto {
protected:
  In_t input;
  Out_t output;
  MetaInfo meta;
  virtual void run() {
    meta.put(UnitInfo{"Unit prototype has been used."});
    if constexpr(std::is_constructible_v<Out_t, int, int, int, int>)
      output = Out_t{1, 0, 1, 0};
  }

public:
  virtual ~UnitProto() = default;
  struct UnitInfo {
    char const *info;
  };
  Out_t operator()(In_t &&data, MetaInfo &info) {
    input = std::move(data);
    meta = std::move(info);
    output.clear();
    run();
    info = std::move(meta);
    return output;
  }
};
#endif // UNITPROTO_HPP