#ifndef UNITPROTO_HXX
#define UNITPROTO_HXX

#include "channel.hxx"
template <typename In_t, typename Out_t> class UnitProto {
public:
  virtual ~UnitProto() = default;
  struct UnitInfo {
    char const* info;
  };
  virtual std::pair<std::vector<Out_t>, MetaInfo> operator()(std::vector<In_t>&& data, MetaInfo&& info) {
    info.put(UnitInfo{"Unit prototype has been used."});

    auto dummy = std::vector<Out_t>{};
    for(auto item : data) {
      dummy.emplace_back(static_cast<Out_t>(item));
    }
    return {dummy, std::move(info)};
  }
};

#endif // UNITPROTO_HXX
