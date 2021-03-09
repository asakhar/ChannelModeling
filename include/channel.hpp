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

struct EmptyObject {};

template <typename Functor> struct result_of { using type = void; };

template <typename Functor>
using result_of_t = typename result_of<Functor>::type;

template <typename Obj> requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Obj> {
  using type = result_of_t<decltype(&Obj::operator())>;
};

template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Out_t (Obj::*)(Ins_t...)> {
  using type = Out_t;
};

template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct result_of<Out_t (Obj::*)(Ins_t...) const> {
  using type = Out_t;
};

template <typename Out_t, typename... Ins_t> struct result_of<Out_t(Ins_t...)> {
  using type = Out_t;
};

template <typename Functor> struct arguments_of { using type = void; };

template <typename Functor>
using arguments_of_t = typename arguments_of<Functor>::type;

template <typename Obj> requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Obj> {
  using type = arguments_of_t<decltype(&Obj::operator())>;
};

template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Out_t (Obj::*)(Ins_t...)> {
  using type = std::tuple<Ins_t...>;
};

template <typename Obj, typename Out_t, typename... Ins_t>
requires requires(Obj &&fn) {
  std::is_convertible_v<decltype(&Obj::operator()), void *>;
}
struct arguments_of<Out_t (Obj::*)(Ins_t...) const> {
  using type = std::tuple<Ins_t...>;
};

template <typename Out_t, typename... Ins_t>
struct arguments_of<Out_t(Ins_t...)> {
  using type = std::tuple<Ins_t...>;
};

template <std::size_t N> struct num { static const constexpr auto value = N; };

template <class F, std::size_t... Is>
constexpr void for_(F func, std::index_sequence<Is...> /**/) {
  (func(num<Is>{}), ...);
}

template <std::size_t N, typename F> constexpr void for_(F func) {
  for_(func, std::make_index_sequence<N>());
}

template <typename... Functors>
requires(
    // (std::is_default_constructible_v<Functors> && ...) &&
    (std::is_copy_constructible_v<Functors> &&...)) class Model

{
public:
  static constexpr size_t N = sizeof...(Functors);

  using In_t = std::tuple_element_t<
      0, arguments_of_t<std::tuple_element_t<0, std::tuple<Functors...>>>>;
  using Out_t =
      result_of_t<std::tuple_element_t<N - 1, std::tuple<Functors...>>>;
  using Tuple_t = std::tuple<std::decay_t<Functors>...>;
  Model(Functors const &...fns) : m_units{fns...} {
    // before call checks
    for_<N - 1>([](auto const i) {
      using In_next =
          std::tuple_element_t<0, arguments_of_t<std::tuple_element_t<
                                      i.value + 1, std::tuple<Functors...>>>>;
      using Out_prev =
          result_of_t<std::tuple_element_t<i.value, std::tuple<Functors...>>>;
      static_assert(std::is_convertible_v<Out_prev, In_next>,
                    "Serial data input/output types must be the same.");
    });
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

  Out_t operator()(In_t input, MetaInfo &meta) {
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::forward<In_t>(input), meta);
  }

  Out_t operator()(In_t input) {
    MetaInfo meta;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::forward<In_t>(input), meta);
  }

  template <bool _ = true>
  requires std::is_same_v<EmptyObject &&, In_t> Out_t operator()() {
    EmptyObject eo;
    MetaInfo meta;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::move(eo), meta);
  }

  template <bool _ = true>
  requires std::is_same_v<EmptyObject &&, In_t>
      Out_t operator()(MetaInfo &meta) {
    EmptyObject eo;
    return Model<Functors...>::call_all<In_t, std::tuple_size_v<Tuple_t>>(
        m_units, std::move(eo), meta);
  }

private:
  Tuple_t m_units;
};

#endif // CHANNEL_HPP
