#include "../include/channel.hpp"
#include "metainfo.hxx"

#include <iostream>

int main(int /*argc*/, char const * /*argv*/[]) {
  // auto l = [](){return 1;};
  // result_of<decltype(m)>::type a = 3;
  auto l1 = [](int v, MetaInfo &meta) mutable -> int {
    meta.put("std::any value");
    return v;
  };
  auto l2 = [](int v) -> bool { return (bool)v; };
  auto l3 = [](bool /*v*/, MetaInfo &meta) -> char const * {
    std::puts(meta.get<char const *>());
    return "abc";
  };
  // typename arguments_of_<decltype(l1)>::type a1;
  // std::tuple_element_t<0, decltype(a1)>;
  // const auto* ptr  = l1.operator();
  // arguments_of_t<decltype(l1)> a;
  // arguments_of<decltype(&decltype(l1)::operator())>::type a;

  Model a(l1, l2, l3);

  std::cout << a(0) << std::endl;
  return 0;
}