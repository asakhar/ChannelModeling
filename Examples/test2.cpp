#include "channel.hpp"
#include "BasicUnits/BinaryGenerator.hpp"
#include "BasicUnits/BinarySymmetricChannel.hpp"

int main(int /*argc*/, char const* /*argv*/[])
{
  auto const dummy = 0.1;
  auto const dummy1 = 100;
  Model model(BinaryGenerator{dummy, dummy1});
  auto out = model();
  for(auto i:out) 
    std::cout << (bool)i << " ";
  std::cout << std::endl;
  return 0;
}


int main1(int /*argc*/, char const * /*argv*/[]) {

  
  auto l1 = [](int v, MetaInfo &meta) mutable -> int {
    meta.put("std::any value");
    return v;
  };
  auto l2 [[maybe_unused]]  = [](int v) -> bool { return (bool)v; };
  auto l3 [[maybe_unused]] = [](bool /*v*/, MetaInfo &meta) -> char const * {
    std::puts(meta.get<char const *>());
    return "abc";
  };

  struct ABS {
    int operator()(int v) {
      return std::abs(v);
    }
  } l4;

  Model a(l1, l4);

  std::cout << a(-4) << std::endl;
  return 0;
}