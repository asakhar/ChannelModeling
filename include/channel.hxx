#ifndef CHANNEL_HXX
#define CHANNEL_HXX

#include <c++/10/any>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>
#if __cplusplus > 201703L && __cpp_concepts >= 201907L
#include <concepts>
#endif
#include "metainfo.hxx"
#include <cstring>
#include <functional>
#include <list>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
// template <typename In_t, typename Out_t> class Model;
using namespace std::string_literals;

class TranslationData {
public:
  template <typename Item_t>
  TranslationData(std::vector<Item_t> &&data, MetaInfo &&info)
      : m_data{std::make_any<std::vector<Item_t>>(
            std::forward<std::vector<Item_t>>(data))},
        m_info{std::forward<MetaInfo>(info)} {}
  template <typename Item_t>
  std::vector<std::remove_reference_t<Item_t>> &get() {
    return std::any_cast<std::vector<std::remove_reference_t<Item_t>> &>(
        m_data);
  }
  MetaInfo &get_info() { return m_info; }
  char const* get_type_name() const{
    return m_data.type().name();
  }

private:
  std::any m_data;
  MetaInfo m_info;
};

class BasicUnit {
public:
  virtual TranslationData operator()(TranslationData &) = 0;
  virtual ~BasicUnit() = default;
  virtual char const* get_in_name() const = 0;
  virtual char const* get_out_name() const = 0;
};

template <typename In_t, typename Out_t>
class ProcessorUnit : public BasicUnit {
public:
  template <typename T, typename Y> friend class Model;
  using Return_t = std::pair<std::vector<Out_t>, MetaInfo>;
  using Input_t = std::vector<In_t>;
  using HandlerSignature = Return_t(Input_t &&, MetaInfo &&);
  ProcessorUnit(std::function<HandlerSignature> hand) : m_handler{hand} {}
  TranslationData operator()(TranslationData &data) override {
    try {
      auto &val = data.get<In_t>();
      auto &inf = data.get_info();
      auto ret = m_handler(std::move(val), std::move(inf));
      return {std::move(ret.first), std::move(ret.second)};
    } catch (std::bad_any_cast &e) {
      throw std::logic_error("Serial units in&out type mismatch ("s + typeid(std::vector<In_t>).name() +" != "s + data.get_type_name() + ").");
    }
  }
  char const* get_in_name() const override {
    return typeid(In_t).name();
  }
  char const* get_out_name() const override {
    return typeid(Out_t).name();
  }

private:
  std::function<HandlerSignature> m_handler;
};


template <typename Ty> concept Convertible_to_function = requires(Ty ty) {
  std::function(ty);
};

template <typename In_t, typename Out_t> class Model {
public:
  Model<In_t, Out_t> &operator>>(Convertible_to_function auto invocable) {
    insert(std::function{invocable});
    return *this;
  }
  template <typename In, typename Out>
  Model<In_t, Out_t> &operator>>(
      std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In> &&,
                                                          MetaInfo &&)>
          model_step) {
    insert(model_step);
    return *this;
  }
template <typename In, typename Out>
  void
  insert(std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In>&&)> model_step) {
    if (m_units.empty() && typeid(In) != typeid(In_t))
      throw std::logic_error("First unit and model input type mismatch ("s +
                             typeid(In_t).name() + " != "s +
                             typeid(void).name() + ")."s);
    auto func = [model_step](std::vector<In> &&v, MetaInfo &&info) {
      auto ret = model_step(std::move(v));
      for (auto &item : ret.second)
        info.put(item.second);
      return std::pair{std::move(ret.first), std::move(info)};
    };
    auto new_unit = std::make_unique<ProcessorUnit<bool, Out>>(std::function(func));
    m_units.emplace_back(std::move(new_unit));
  }

  template <typename Out>
  void
  insert(std::function<std::pair<std::vector<Out>, MetaInfo>()> model_step) {
    if (m_units.empty() && typeid(void) != typeid(In_t))
      throw std::logic_error("First unit and model input type mismatch ("s +
                             typeid(In_t).name() + " != "s +
                             typeid(void).name() + ")."s);
    auto func = [model_step](std::vector<EmptyObject> &&v, MetaInfo &&info) {
      auto ret = model_step();
      for (auto &item : ret.second)
        info.put(item.second);
      return std::pair{std::move(ret.first), std::move(info)};
    };
    auto new_unit = std::make_unique<ProcessorUnit<EmptyObject, Out>>(std::function(func));
    m_units.emplace_back(std::move(new_unit));
  }
  template <typename In, typename Out>
  void
  insert(std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In> &&,
                                                             MetaInfo &&)>
             model_step) {
    if (m_units.empty() && typeid(In) != typeid(In_t))
      throw std::logic_error("First unit and model input type mismatch ("s +
                             typeid(In_t).name() + " != "s + typeid(In).name() +
                             ")."s);
    auto new_unit = std::make_unique<ProcessorUnit<In, Out>>(model_step);
    m_units.emplace_back(std::move(new_unit));
  }
  std::vector<Out_t> operator()() {
    if (typeid(In_t) != typeid(void))
      throw std::logic_error(
          "Can't invoke model without params as it takes some.");
    if (m_units.empty())
      return std::vector<Out_t>{};
    TranslationData current{std::vector<EmptyObject>{}, MetaInfo{}};
    for (auto &item : m_units) {
      current = (*item)(current);
    }
    try {
      return current.get<Out_t>();
    } catch (std::bad_any_cast &e) {
      throw std::logic_error(
          "Last unit return type is invalid for this model ("s+m_units.back()->get_out_name()+" != "s+ typeid(Out_t).name()+")."s);
    }
  }
  std::vector<Out_t> operator()(std::vector<In_t> &&input) {
    if (m_units.empty())
      return std::vector<Out_t>{};
    TranslationData current{std::move(input), MetaInfo{}};
    for (auto &item : m_units) {
      current = (*item)(current);
    }
    try {
      return current.get<Out_t>();
    } catch (std::bad_any_cast &e) {
      throw std::logic_error(
          "Last unit return type is invalid for this model ("s+m_units.back()->get_out_name()+" != "s+ typeid(Out_t).name()+")."s);
    }
  }

private:
  struct EmptyObject {};
  std::list<std::unique_ptr<BasicUnit>> m_units;
};

template <typename In, typename Out> struct Processor1 {
public:
  template <typename In1, typename Out1>
  Processor1<In, Out1>
  operator>>(std::function<std::vector<Out1>(std::vector<In1>)> fn) & {
    return Processor1<In, Out1>{[this, fn](std::vector<In> in, MetaInfo info) {
      auto [in1, info1] = m_proc(in, info);
      return std::pair{fn(in1), info1};
    }};
  }
  std::pair<std::vector<Out>, MetaInfo> operator()(std::vector<In> in,
                                                   MetaInfo info) {
    return m_proc(in, info);
  }
  std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In>,
                                                      MetaInfo)>
      m_proc;
};

class Decorator {

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

  // template <typename Fn> Decorator &operator>>(std::function<Fn>) {}

  Decorator &operator>>(Decorator &next);

#if __cplusplus > 201703L && __cpp_concepts >= 201907L
  template <std::derived_from<Decorator> Dec>
#else
  template <typename Dec>
#endif
  Decorator &operator>>(Dec &&next) {
    m_next = new Dec{std::forward<Dec>(next)};
    m_next_on_heap = true;
    return *m_next;
  }

protected:
  class RunReturn {
  public:
    RunReturn(Decorator *dec, Decorator::SmartArr_t &&fwd, size_t bs);
    inline operator Decorator::Array_t() { return std::move(res); }

  private:
    Decorator::Array_t res;
  };

  virtual RunReturn run(SmartArr_t data, size_t block_size) = 0;
  inline Array_t runNext(SmartArr_t data, size_t block_size) {
    if (m_next != nullptr)
      return m_next->run(std::forward<SmartArr_t>(data), block_size);
    return {std::forward<SmartArr_t>(data), block_size};
  }

private:
  Decorator *m_next = 0;
  bool m_next_on_heap = false;
};

class RepetitionEncoder : public Decorator {
public:
  constexpr inline RepetitionEncoder(size_t N) { setN(N); }

  constexpr inline void setN(size_t N) {
    n = N;
#ifndef NDEBUG
    if (!(n % 2))
      std::cerr << "[Warning] In call to RepetitonEncoder{N}: N=" << n
                << " is not odd.\n";
#endif
  }
  constexpr inline size_t getN() const { return n; }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class RepetitionDecoder : public Decorator {
public:
  constexpr inline RepetitionDecoder(size_t N) { setN(N); }

  constexpr inline void setN(size_t N) {
    n = N;
#ifndef NDEBUG
    if (!(n % 2))
      std::cerr << "[Warning] In call to RepetitonEncoder{N}: N=" << n
                << " is not odd.\n";
#endif
  }
  constexpr inline size_t getN() const { return n; }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class ParityCheckEncoder : public Decorator {
public:
  constexpr inline ParityCheckEncoder(size_t N) { setN(N); }

  constexpr inline void setN(size_t N) { n = N; }

  constexpr inline size_t getN() const { return n; }

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  size_t n = 0;
};

class BinarySymmetricChannel : public Decorator {
public:
  BinarySymmetricChannel(double bitflip_prob);

protected:
  RunReturn run(SmartArr_t data, size_t block_size) override;

private:
  double p;
  std::bernoulli_distribution m_bern_distr;
};

#endif // CHANNEL_HXX
