#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "metainfo.hxx"
#include <any>
#include <array>
#include <concepts>
#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>
/**
 * @brief Object type for units-generators (which does not take any parameters)
 *
 */
struct EmptyObject {};

/**
 * @brief helper struct for getting the returning type of callable
 *
 * @tparam Functor
 */
template <typename Functor> struct result_of { using type = void; };

/**
 * @brief templated alias for getting the returning type of callable
 *
 * @tparam Functor
 */
template <typename Functor>
using result_of_t = typename result_of<Functor>::type;

/**
 * @brief helper struct for getting the returning type of callable
 *
 * @tparam Functor
 */
template <typename Obj> requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Obj> {
  using type = result_of_t<decltype(&Obj::operator())>;
};

/**
 * @brief helper struct for getting the returning type of callable
 *
 * @tparam Functor
 */
template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Out_t (Obj::*)(Ins_t...)> {
  using type = Out_t;
};

/**
 * @brief helper struct for getting the returning type of const callable
 *
 * @tparam Functor
 */
template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Out_t (Obj::*)(Ins_t...) const> {
  using type = Out_t;
};

/**
 * @brief helper struct for getting the returning type of function
 *
 * @tparam Functor
 */
template <typename Out_t, typename... Ins_t> struct result_of<Out_t(Ins_t...)> {
  using type = Out_t;
};

/**
 * @brief helper struct for getting argument types of callable
 *
 * @tparam Functor
 */
template <typename Functor> struct arguments_of { using type = void; };
/**
 * @brief templated alias for getting argument types of callable
 *
 * @tparam Functor
 */
template <typename Functor>
using arguments_of_t = typename arguments_of<Functor>::type;
/**
 * @brief helper struct for getting argument types of callable
 *
 * @tparam Functor
 */
template <typename Obj> requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Obj> {
  using type = arguments_of_t<decltype(&Obj::operator())>;
};
/**
 * @brief helper struct for getting argument types of callable
 *
 * @tparam Functor
 */
template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Out_t (Obj::*)(Ins_t...)> {
  using type = std::tuple<Ins_t...>;
};
/**
 * @brief helper struct for getting argument types of const callable
 *
 * @tparam Functor
 */
template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Out_t (Obj::*)(Ins_t...) const> {
  using type = std::tuple<Ins_t...>;
};
/**
 * @brief helper struct for getting argument types of function
 *
 * @tparam Functor
 */
template <typename Out_t, typename... Ins_t>
struct arguments_of<Out_t(Ins_t...)> {
  using type = std::tuple<Ins_t...>;
};
/**
 * @brief Model class
 *
 * @tparam Functors
 */
template <typename... Functors>
requires((std::is_copy_constructible_v<Functors> ||
          std::is_move_constructible_v<Functors>)&&...) class Model

{
public:
  /**
   * @brief Number of units
   *
   */
  static constexpr size_t N = sizeof...(Functors);
  /**
   * @brief first unit input type (model input type)
   *
   */
  using In_t = std::tuple_element_t<
      0, arguments_of_t<std::tuple_element_t<0, std::tuple<Functors...>>>>;
  /**
   * @brief Last unit output type (model output type)
   *
   */
  using Out_t =
      result_of_t<std::tuple_element_t<N - 1, std::tuple<Functors...>>>;
  /**
   * @brief Alias for tuple type that contains all units of this model
   *
   */
  using Tuple_t = std::tuple<std::decay_t<Functors>...>;
  /**
   * @brief Construct a new Model object
   *
   * @param fns functors or generic callbles
   */
  Model(Functors... fns) : m_units{fns...} {
    // before call checks
    for_<N - 1>([](auto const i) {
      using In_next = std::tuple_element_t<
          0, arguments_of_t<std::tuple_element_t<i.value + 1, Tuple_t>>>;
      using Out_prev = result_of_t<std::tuple_element_t<i.value, Tuple_t>>;
      static_assert(std::is_convertible_v<Out_prev, In_next>,
                    "Serial data input/output types must be the same.");
    });
  }
  /**
   * @brief Run model with input and pass user meta info
   *
   * @param input data to process
   * @param meta
   * @return Out_t
   */
  Out_t operator()(In_t input, MetaInfo &meta) {
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::forward<In_t>(input), meta);
  }
  /**
   * @brief Run model with input
   *
   * @param input data to process
   * @return Out_t
   */
  Out_t operator()(In_t input) {
    MetaInfo meta;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::forward<In_t>(input), meta);
  }
  /**
   * @brief Run model without input (only avaliable if first unit takes
   * EmptyObject as an argument)
   *
   */
  template <bool _ = true>
  requires std::is_same_v<EmptyObject &&, In_t> Out_t operator()() {
    EmptyObject eo;
    MetaInfo meta;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::move(eo), meta);
  }
  /**
   * @brief Run model without input but pass user meta info (only avaliable if
   * first unit takes EmptyObject as an argument)
   *
   * @param meta
   */
  template <bool _ = true>
  requires std::is_same_v<EmptyObject &&, In_t>
      Out_t operator()(MetaInfo &meta) {
    EmptyObject eo;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::move(eo), meta);
  }
  template<size_t i>
  auto& get() {
    return std::get<i>(m_units);
  }

private:
  template <std::size_t N> struct num {
    static const constexpr auto value = N;
  };

  template <class F, std::size_t... Is>
  static constexpr void for_(F func, std::index_sequence<Is...> /**/) {
    (func(num<Is>{}), ...);
  }

  template <std::size_t N, typename F> static constexpr void for_(F func) {
    for_(func, std::make_index_sequence<N>());
  }

  template <typename In, size_t idx>
  static decltype(auto) call_all(Tuple_t &functors, In &&in, MetaInfo &meta) {
    if constexpr (idx == 0)
      return std::forward<In>(in);
    if constexpr (idx != 0) {
      using CurrentFunc = std::tuple_element_t<idx - 1, Tuple_t>;
      using CurrentArgs = arguments_of_t<CurrentFunc>;
      if constexpr (std::tuple_size_v<CurrentArgs> == 0) {
        return std::get<idx - 1>(functors)();
      }
      if constexpr (std::tuple_size_v<CurrentArgs> == 1) {
        if constexpr (std::is_same_v<std::tuple_element_t<0, Tuple_t>,
                                     MetaInfo &>)
          return std::get<idx - 1>(functors)(meta);
        return std::get<idx - 1>(functors)(
            call_all<In, idx - 1>(functors, std::forward<In>(in), meta));
      }
      if constexpr (std::tuple_size_v<CurrentArgs> == 2) {
        return std::get<idx - 1>(functors)(
            Model<Functors...>::call_all<In, idx - 1>(
                functors, std::forward<In>(in), meta),
            meta);
      }
    }
  }
  Tuple_t m_units;
};

#endif // CHANNEL_HPP
