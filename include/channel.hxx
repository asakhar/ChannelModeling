#ifndef CHANNEL_HXX
#define CHANNEL_HXX

#include <random>
#include <iostream>
#include <string>
#if __cplusplus > 201703L && __cpp_concepts >= 201907L
#include <concepts>
#endif
#include <functional>
#include <memory>
#include <sstream>
#include <cstring>
#include <string_view>

class Decorator
{

public:
  using Data_t = uint_fast8_t;
  using SmartArr_t = std::unique_ptr<Data_t[]>;
  using Array_t = std::pair<SmartArr_t, size_t>;
  constexpr Decorator() = default;
  virtual ~Decorator();

  Array_t evaluate(Data_t const *data, size_t block_size);

#if __cplusplus > 201703L && __cpp_concepts >= 201907L
  Array_t evaluate(std::basic_string_view<Data_t> data);
#endif
  Array_t evaluate(std::vector<Data_t> const &data);

  Decorator &operator>>(Decorator &next);

#if __cplusplus > 201703L && __cpp_concepts >= 201907L
  template <std::derived_from<Decorator> Dec>
#else
  template <typename Dec>
#endif
  Decorator &operator>>(Dec &&next)
  {
    m_next = new Dec{std::forward<Dec>(next)};
    m_next_on_heap = true;
    return *m_next;
  }

protected:
  class RunReturn
  {
  public:
    RunReturn(Decorator *dec, Decorator::SmartArr_t &&fwd, size_t bs);
    inline operator Decorator::Array_t()
    {
      return std::move(res);
    }

  private:
    Decorator::Array_t res;
  };

  virtual RunReturn run(SmartArr_t data, size_t block_size) = 0;
  inline Array_t runNext(SmartArr_t data, size_t block_size)
  {
    if (m_next)
      return m_next->run(std::forward<SmartArr_t>(data), block_size);
    else
      return {std::forward<SmartArr_t>(data), block_size};
  }

private:
  Decorator *m_next = 0;
  bool m_next_on_heap = 0;
};

class RepetitionEncoder : public Decorator
{
public:
  constexpr inline RepetitionEncoder(size_t N)
  {
    setN(N);
  }

  constexpr inline void setN(size_t N)
  {
    n = N;
#ifndef NDEBUG
    if (!(n % 2))
      std::cerr << "[Warning] In call to RepetitonEncoder{N}: N=" << n << " is not odd.\n";
#endif
  }
  constexpr inline size_t getN() const
  {
    return n;
  }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class RepetitionDecoder : public Decorator
{
public:
  constexpr inline RepetitionDecoder(size_t N)
  {
    setN(N);
  }

  constexpr inline void setN(size_t N)
  {
    n = N;
#ifndef NDEBUG
    if (!(n % 2))
      std::cerr << "[Warning] In call to RepetitonEncoder{N}: N=" << n << " is not odd.\n";
#endif
  }
  constexpr inline size_t getN() const
  {
    return n;
  }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class ParityCheckEncoder : public Decorator
{
public:
  constexpr inline ParityCheckEncoder(size_t N)
  {
    setN(N);
  }

  constexpr inline void setN(size_t N)
  {
    n = N;
  }

  constexpr inline size_t getN() const
  {
    return n;
  }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class BinarySymmetricChannel : public Decorator
{
public:
  BinarySymmetricChannel(double bitflip_prob);

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  double p;
  std::bernoulli_distribution m_bern_distr;
};

#endif // CHANNEL_HXX
