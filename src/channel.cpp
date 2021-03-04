#include "../include/channel.hpp"
#include "metainfo.hxx"

#include <iostream>

int main(int /*argc*/, char const * /*argv*/[]) {
  auto l1 = [](int v, MetaInfo &meta) mutable -> int {
    meta.put("std::any value");
    return v;
  };
  auto l2 = [](int v) -> bool { return (bool)v; };
  auto l3 = [](bool /*v*/, MetaInfo &meta) -> char const * {
    std::puts(meta.get<char const *>());
    return "abc";
  };

  Model a(l1, l2, l3);

  std::cout << a(0) << std::endl;
  return 0;
}