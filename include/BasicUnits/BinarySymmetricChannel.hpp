#ifndef BINARYSYMMETRICCHANNEL_HXX
#define BINARYSYMMETRICCHANNEL_HXX

#include "unitproto.hpp"
#include <random>
#include <vector>
/**
 * @brief Example of binary symmetric channel unit.
 * Randomly injects errors in input data within the given chance
 *
 */
class BinarySymmetricChannel
    : public UnitProto<std::vector<bool>, std::vector<bool>> {
public:
  BinarySymmetricChannel() = default;
  /**
   * @brief Construct a new Binary Symmetric Channel object
   *
   * @param bitflip_chance Chance of injecting an error per one data bit
   * @param seed Seed for pseudo-random number generator
   */
  BinarySymmetricChannel(double bitflip_chance,
                         uint32_t seed = std::random_device{}())
      : p{bitflip_chance}, gen{seed}, dist{0., 1.} {}
  /**
   * @brief Processing method.
   * Exaple shows that You actually are avaliable to move from input data to
   * output in order to save resources
   *
   */
  void run() override {
    output = std::move(input);
    for (auto item : output)
      item = item ^ (dist(gen) <= p);
  }

private:
  double p = 0;
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
};

#endif // BINARYSYMMETRICCHANNEL_HXX
