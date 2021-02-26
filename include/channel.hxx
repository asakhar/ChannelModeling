#ifndef CHANNEL_HXX
#define CHANNEL_HXX

#include <any>
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
using namespace std::string_literals;

class TranslationData {
public:
  template <typename Item_t>
  constexpr TranslationData(std::vector<Item_t> &&data, MetaInfo &&info)
      : m_data{std::make_any<std::vector<Item_t>>(
            std::forward<std::vector<Item_t>>(data))},
        m_info{std::forward<MetaInfo>(info)} {}
  template <typename Item_t>
  constexpr std::vector<std::remove_reference_t<Item_t>> &get() {
    return std::any_cast<std::vector<std::remove_reference_t<Item_t>> &>(
        m_data);
  }
  constexpr inline MetaInfo &get_info() { return m_info; }
  constexpr inline char const *get_type_name() const {
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
  virtual char const *get_in_name() const = 0;
  virtual char const *get_out_name() const = 0;
  struct EmptyObject {};
};

template <typename In_t, typename Out_t>
class ProcessorUnit : public BasicUnit {
public:
  template <typename T, typename Y> friend class Model;
  using Return_t = std::pair<std::vector<Out_t>, MetaInfo>;
  using Input_t = std::vector<In_t>;
  using HandlerSignature = Return_t(Input_t &&, MetaInfo &&);
  constexpr inline ProcessorUnit(std::function<HandlerSignature> hand)
      : m_handler{hand} {}
  TranslationData operator()(TranslationData &data) override {
    try {
      auto &val = data.get<In_t>();
      auto &inf = data.get_info();
      auto ret = m_handler(std::move(val), std::move(inf));
      return {std::move(ret.first), std::move(ret.second)};
    } catch (std::bad_any_cast &e) {
      throw std::logic_error("Serial units in&out type mismatch ("s +
                             typeid(std::vector<In_t>).name() + " != "s +
                             data.get_type_name() + ").");
    }
  }
  constexpr inline char const *get_in_name() const override {
    return typeid(In_t).name();
  }
  constexpr inline char const *get_out_name() const override {
    return typeid(Out_t).name();
  }

private:
  std::function<HandlerSignature> m_handler;
};

#if __cplusplus > 201703L && __cpp_concepts >= 201907L
template <typename Ty> concept Convertible_to_function = requires(Ty ty) {
  std::function(ty);
};
#endif

template <typename In_t, typename Out_t> class Model {
public:
  constexpr Model<In_t, Out_t> &operator>>(
#if __cplusplus > 201703L && __cpp_concepts >= 201907L
      Convertible_to_function
#endif
      auto invocable) {
    insert(std::function{invocable});
    return *this;
  }
  template <typename In, typename Out>
  constexpr Model<In_t, Out_t> &operator>>(
      std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In> &&,
                                                          MetaInfo &&)>
          model_step) {
    insert(model_step);
    return *this;
  }
  template <typename In, typename Out>
  constexpr void insert(
      std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In> &&)>
          model_step) {
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
    auto new_unit =
        std::make_unique<ProcessorUnit<bool, Out>>(std::function(func));
    m_units.emplace_back(std::move(new_unit));
  }

  template <typename Out>
  constexpr void
  insert(std::function<std::pair<std::vector<Out>, MetaInfo>()> model_step) {
    if (m_units.empty() && typeid(void) != typeid(In_t))
      throw std::logic_error("First unit and model input type mismatch ("s +
                             typeid(In_t).name() + " != "s +
                             typeid(void).name() + ")."s);
    auto func = [model_step](std::vector<BasicUnit::EmptyObject> && /*v*/,
                             MetaInfo &&info) {
      auto ret = model_step();
      for (auto &item : ret.second)
        info.put(item.second);
      return std::pair{std::move(ret.first), std::move(info)};
    };
    auto new_unit =
        std::make_unique<ProcessorUnit<BasicUnit::EmptyObject, Out>>(
            std::function(func));
    m_units.emplace_back(std::move(new_unit));
  }
  template <typename In, typename Out>
  constexpr void
  insert(std::function<std::pair<std::vector<Out>, MetaInfo>(std::vector<In> &&,
                                                             MetaInfo &&)>
             model_step) {
    if (m_units.empty() && typeid(In) != typeid(In_t) &&
        (typeid(In_t) != typeid(void) &&
         typeid(In) != typeid(BasicUnit::EmptyObject)))
      throw std::logic_error("First unit and model input type mismatch ("s +
                             typeid(In_t).name() + " != "s + typeid(In).name() +
                             ")."s);
    auto new_unit = std::make_unique<ProcessorUnit<In, Out>>(model_step);
    m_units.emplace_back(std::move(new_unit));
  }
  std::vector<Out_t> operator()() {
    static_assert(std::is_void_v<In_t> ||
                      std::is_same_v<In_t, BasicUnit::EmptyObject>,
                  "Can't invoke model without params as long as it takes some.");
    if (m_units.empty())
      return std::vector<Out_t>{};
    TranslationData current{std::vector<BasicUnit::EmptyObject>{}, MetaInfo{}};
    for (auto &item : m_units) {
      current = (*item)(current);
    }
    try {
      return current.get<Out_t>();
    } catch (std::bad_any_cast &e) {
      throw std::logic_error(
          "Last unit return type is invalid for this model ("s +
          m_units.back()->get_out_name() + " != "s + typeid(Out_t).name() +
          ")."s);
    }
  }
  std::vector<Out_t> operator()(std::vector<In_t> input) {
    static_assert(!std::is_void_v<In_t> &&
                      !std::is_same_v<In_t, BasicUnit::EmptyObject>,
                  "Can't invoke model with params as long as it takes none.");
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
          "Last unit return type is invalid for this model ("s +
          m_units.back()->get_out_name() + " != "s + typeid(Out_t).name() +
          ")."s);
    }
  }

private:
  std::list<std::unique_ptr<BasicUnit>> m_units;
};

#endif // CHANNEL_HXX
