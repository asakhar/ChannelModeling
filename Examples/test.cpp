#include "BasicUnits/BinaryGenerator.hpp"
#include "channel.hpp"
#include "metainfo.hxx"
#include "unitproto.hpp"
#include <exception>
#include <functional>
#include <sstream>
#include <vector>
int test();

int main(int /*argc*/, char const * /*argv*/[]) {
  UnitProto<std::vector<bool>, std::vector<bool>> proto;

  auto without_meta = [i = 1](std::vector<bool> &&data) mutable {
    auto meta = MetaInfo{};
    std::stringstream ss;
    ss << "i=" << i++;
    meta.put(ss.str().c_str());
    return std::move(data);
  };
  auto passthrough = [](std::vector<bool> &&data, MetaInfo &info) {
    info.put("some meta");
    std::cout
        << info.get<UnitProto<std::vector<bool>, std::vector<bool>>::UnitInfo>()
               .info
        << "\n"
        << info.get<char const *>() << std::endl;
    return std::move(data);
  };

  double const prob = 0.6;
  size_t const number = 20;
  BinaryGenerator bingen(prob, number);
  Model model2{bingen};

  auto ret2 = model2();
  for (auto item : ret2)
    std::cout << item << " ";
  std::cout << std::endl;

  auto cast = [](std::vector<bool> &&data) {
    std::vector<double> tmp;
    auto const dummy = 0.1;
    for (auto item : data)
      tmp.emplace_back((double)item + dummy);
    return tmp;
  };
  auto increment = [](std::vector<double> &&data, MetaInfo &info) {
    for (auto &item : data)
      item += 1.;
    std::cout << (info.find<char const*>() != std::end(info) ? info.get<char const *>() : "") << "\n"
              << (info.find<char>() != std::end(info) ? info.get<char>() : ' ')
              << std::endl;
    return std::move(data);
  };
  Model model{proto, passthrough, without_meta, cast, increment};
  auto ret = model({true, false, true});
  for (auto item : ret)
    std::cout << item << " ";
  std::cout << std::endl;
  ret = model({true, false, true});
  for (auto item : ret)
    std::cout << item << " ";
  std::cout << std::endl;
  return 0;
}
