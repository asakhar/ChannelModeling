#ifndef UNITPROTO_HXX
#define UNITPROTO_HXX

#include "channel.hxx"
#include "metainfo.hxx"
#include <random>
template <typename In_t, typename Out_t> class UnitProto {
protected:
  std::vector<In_t> input;
  std::vector<Out_t> output;
  MetaInfo meta;
  virtual void run() {
    meta.put(UnitInfo{"Unit prototype has been used."});
    output = std::vector<Out_t>{1, 0, 1, 0};
  }

public:
  virtual ~UnitProto() = default;
  struct UnitInfo {
    char const *info;
  };
  std::pair<std::vector<Out_t>, MetaInfo> operator()(std::vector<In_t> &&data,
                                                     MetaInfo &&info) {
    input = std::move(data);
    meta = std::move(info);
    output.clear();
    run();
    return {std::move(output), std::move(meta)};
  }
};

#endif // UNITPROTO_HXX
