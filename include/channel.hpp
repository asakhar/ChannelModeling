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

template <typename... Functors> class Model {
public:
  static constexpr size_t N = sizeof...(Functors);
  using In_t = std::tuple_element_t<
      0, arguments_of_t<std::tuple_element_t<0, std::tuple<Functors...>>>>;
  using Out_t =
      result_of_t<std::tuple_element_t<N - 1, std::tuple<Functors...>>>;
  Model(Functors const &...fns) {
    std::tuple<Functors...> m_units{fns...};
    for_<N - 1>([](auto const i) {
      using In_next =
          std::tuple_element_t<0, arguments_of_t<std::tuple_element_t<
                                      i.value + 1, std::tuple<Functors...>>>>;
      using Out_prev =
          result_of_t<std::tuple_element_t<i.value, std::tuple<Functors...>>>;
      static_assert(std::is_convertible_v<std::decay_t<Out_prev>, In_next>,
                    "Serial data input/output types must be the same.");
    });
  }

  Out_t operator()(In_t input) const {
    std::any in_next = std::make_any<In_t>(std::move(input));
    MetaInfo meta{};
    for_<N>([this, &in_next, &meta](auto const i) {
      using Now_in_t =
          std::tuple_element_t<0, arguments_of_t<std::tuple_element_t<
                                      i.value, std::tuple<Functors...>>>>;
      constexpr size_t arg_size = std::tuple_size_v<arguments_of_t<
          std::tuple_element_t<i.value, std::tuple<Functors...>>>>;
      if constexpr (arg_size == 2) {
        using Second_arg =
            std::tuple_element_t<1, arguments_of_t<std::tuple_element_t<
                                        i.value, std::tuple<Functors...>>>>;
        static_assert(std::is_same_v<Second_arg, MetaInfo&>,
                      "Second arg must be MetaInfo reference.");
        // if constexpr (i.value + 1 != N) {
          using Next_in_t = std::tuple_element_t<
              0, arguments_of_t<std::tuple_element_t<i.value + 1,
                                                     std::tuple<Functors..., void(Out_t)>>>>;
          in_next = std::make_any<Next_in_t>(std::move(
              std::get<i.value>(m_units)(std::any_cast<Now_in_t>(in_next), meta)));
        // } else {
        //   in_next = std::make_any<Out_t>(std::move(
        //       std::get<i.value>(m_units)(std::any_cast<Now_in_t>(in_next), meta)));
        // }
      } else if constexpr (arg_size == 1) {
        // if constexpr (i.value + 1 != N) {
          using Next_in_t = std::tuple_element_t<
              0, arguments_of_t<std::tuple_element_t<i.value + 1,
                                                     std::tuple<Functors..., void(Out_t)>>>>;
          in_next = std::make_any<Next_in_t>(std::move(
              std::get<i.value>(m_units)(std::any_cast<Now_in_t>(in_next))));
        // } else {
        //   in_next = std::make_any<Out_t>(std::move(
        //       std::get<i.value>(m_units)(std::any_cast<Now_in_t>(in_next))));
        // }
      }
    });
    return std::any_cast<Out_t>(in_next);
  }

private:
  std::tuple<Functors...> m_units;
};

#endif // CHANNEL_HPP
