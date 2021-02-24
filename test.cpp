#include "include/channel.hxx"
#include "metainfo.hxx"
#include <functional>
#include <vector>
int test();

int main(int argc, char const *argv[]) {
  Model<bool, double> model;
  auto passthrough = [](std::vector<bool> &&data, MetaInfo &&info) {
    info.put("some meta");
    return std::pair{data, info};
  };
  auto cast = [](std::vector<bool> &&data, MetaInfo &&info) {
    std::vector<double> tmp;
    for (auto item : data)
      tmp.emplace_back((double)item+0.1);
    return std::pair{std::move(tmp), info};
  };
  auto increment = [](std::vector<double> &&data, MetaInfo &&info) {
    for(auto&item:data)
      item += 1.;
    std::cout << info.get<char const*>() << std::endl;
    return std::pair{std::move(data), info};
  };
  auto cast_back = [](std::vector<double> &&data, MetaInfo &&info) {
    std::vector<bool> tmp;
    for (auto item : data)
      tmp.emplace_back((bool)item);
    return std::pair{std::move(tmp), info};
  };
  model >> passthrough  >> cast >> increment;
  // model >> passthrough >> increment;
  // model >> std::function(passthrough) >> std::function(cast) >> increment;
  auto ret = model({true, false, true});
  for (auto item : ret)
    std::cout << item << " ";
  std::cout << std::endl;
  // Processor<bool, bool> a{[](auto sig, MetaInfo info) {
  //   sig[0] = 1;
  //   sig[1] = 0;
  //   return std::pair{sig, info};
  // }};
  // auto worker =
  //     a >> std::function{[](std::vector<bool> sig) {
  //       std::vector<double> ret;
  //       ret.resize(sig.size());
  //       std::random_device rd;
  //       std::mt19937 gen{rd()};
  //       for (auto i = 0; auto &item : ret) {
  //         item = (double)sig[i++] + std::fmod(((double)gen()) / 100., 1.);
  //       }
  //       return ret;
  //     }};
  // auto res = worker({false,true,false,false,false,false}, MetaInfo{});
  // for(auto &i : res.first) {
  //   std::cout << i << " ";
  // }
  // std::cout << std::endl;
  // RepetitionDecoder a{4};
  // test();
  // ParityCheckEncoder rep{3};
  // auto res = rep.evaluate({0, 0, 1, 1, 0, 1});
  // for (size_t i = 0; i < res.second; i++)
  //   std::cout << (bool)res.first[i];
  // std::cout << std::endl;
  return 0;
}

#include <random>

int test() {
  int const repeat_times = 5;
  double const flipping_chance = 0.01;
  double const coin_flip_prob = 0.5;
  int const vec_size = 50;
  int const max_errors = 50;

  RepetitionEncoder enc{repeat_times};
  enc >> BinarySymmetricChannel{flipping_chance} >>
      RepetitionDecoder{repeat_times};

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::bernoulli_distribution bern_distr{coin_flip_prob};
  auto fill_arr = [&bern_distr, &gen](auto &arr) {
    for (auto &item : arr)
      item = bern_distr(gen);
  };

  int errs = 0;
  std::vector<uint_fast8_t> vec{};
  vec.resize(vec_size, 0);
  int iters = 0;
  while (errs < max_errors) {
    fill_arr(vec);
    auto res = enc.evaluate(vec);

    // ##################################
    // std::cout << "\nResolved:\n";
    // for (size_t i = 0; i < res.second; i++)
    // std::cout << (bool)res.first[i];
    // std::cout << "\nActual:\n";

    // ##################################

    for (size_t i = 0; auto &item : vec) {
      // std::cout << (bool)item;
      if (item != res.first[i++])
        errs++;
    }
    iters++;
  }
  std::cout << "\nIterations=" << iters << std::endl;

  return 0;
}
