#ifndef UNITPROTO_HXX
#define UNITPROTO_HXX

#include "channel.hxx"
#include "metainfo.hxx"
#include <random>
template <typename In_t, typename Out_t> class UnitProto {
public:
  virtual ~UnitProto() = default;
  struct UnitInfo {
    char const *info;
  };
  virtual std::pair<std::vector<Out_t>, MetaInfo>
  operator()(std::vector<In_t> &&data, MetaInfo &&info) {
    info.put(UnitInfo{"Unit prototype has been used."});

    auto dummy = std::vector<Out_t>{};
    return {dummy, std::move(info)};
  }
  // virtual std::pair<std::vector<Out_t>, MetaInfo>
  // operator()() {
  //   MetaInfo info;
  //   info.put(UnitInfo{"Unit prototype has been used."});

  //   auto dummy = std::vector<Out_t>{};
  //   return {dummy, std::move(info)};
  // }
  // virtual std::pair<std::vector<Out_t>, MetaInfo>
  // operator()(std::vector<In_t> &&data) {
  //   MetaInfo info;
  //   info.put(UnitInfo{"Unit prototype has been used."});

  //   auto dummy = std::vector<Out_t>{};
  //   return {dummy, std::move(info)};
  // }
};

class BinaryGenerator : public UnitProto<BasicUnit::EmptyObject, bool> {
public:
  struct GeneratorOut {
    std::vector<bool> data;
  };
  BinaryGenerator(double probability, size_t count) : p{probability}, n{count} {}
  std::pair<std::vector<bool>, MetaInfo> operator()(std::vector<BasicUnit::EmptyObject> &&data, MetaInfo &&info) override {
    MetaInfo meta{};
    GeneratorOut out;
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::bernoulli_distribution bern{p};
    std::vector<bool> ret;
    ret.resize(n);
    for(auto item:ret) {
      item = bern(gen);
      out.data.emplace_back((bool)item);
    }
    meta.put(out);
    return {std::move(ret), std::move(meta)};
  }
  double p;
  size_t n;
};

#endif // UNITPROTO_HXX
