#include "include/channel.hxx"
int test();

int main(int argc, char const *argv[])
{
  RepetitionDecoder a{4};
  test();
  // ParityCheckEncoder rep{3};
  // auto res = rep.evaluate({0, 0, 1, 1, 0, 1});
  // for (size_t i = 0; i < res.second; i++)
  //   std::cout << (bool)res.first[i];
  // std::cout << std::endl;
  return 0;
}

#include <random>

int test()
{
  RepetitionEncoder enc{5};
  enc >> BinarySymmetricChannel{0.01} >> RepetitionDecoder{5};

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::bernoulli_distribution bern_distr{0.5};
  auto fill_arr = [&bern_distr, &gen](auto &arr) {
    for (auto &item : arr)
      item = bern_distr(gen);
  };

  int errs = 0;
  std::vector<uint_fast8_t> vec{};
  vec.resize(50, 0);
  int iters = 0;
  while (errs < 50)
  {
    fill_arr(vec);
    auto res = enc.evaluate(vec);

    // ##################################
    //std::cout << "\nResolved:\n";
    //for (size_t i = 0; i < res.second; i++)
      //std::cout << (bool)res.first[i];
    //std::cout << "\nActual:\n";

    // ##################################

    for (size_t i = 0; auto &item : vec)
    {
      //std::cout << (bool)item;
      if (item != res.first[i++])
        errs++;
    }
    iters++;
  }
  std::cout << "\nIterations=" << iters << std::endl;

  return 0;
}
