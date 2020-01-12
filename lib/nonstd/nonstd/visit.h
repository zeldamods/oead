/*
 * Copyright Björn Fahller 2018,2019
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/visit
 */

#ifndef ROLLBEAR_VISIT_HPP
#define ROLLBEAR_VISIT_HPP

#include <variant>
#include <utility>
#include <tuple>

namespace rollbear
{
  namespace detail {

    template <typename ... Ts>
    std::variant<Ts...> variant_access_(const std::variant<Ts...>*);

    template <typename T>
    using variant_access = decltype(variant_access_(static_cast<std::decay_t<T>*>(nullptr)));

    template <template <typename ...> class, typename = void, typename ...>
    struct detected : std::false_type {};

    template <template <typename ...> class D, typename ... Ts>
    struct detected<D, std::void_t<D<Ts...>>, Ts...> : std::true_type {};

    template <template <typename ...> class D, typename ... Ts>
      using is_detected = typename detected<D, void, Ts...>::type;

    template <template <typename ...> class D, typename ... Ts>
    constexpr bool is_detected_v = is_detected<D, Ts...>::value;

    template <typename T>
    constexpr bool is_variant_v = is_detected_v<variant_access, T>;

    template<std::size_t I, std::size_t ... Is>
    constexpr
    std::index_sequence<I, Is...>
    prepend(std::index_sequence<Is...>) {
      return {};
    }

    constexpr
    std::index_sequence<>
    next_seq(std::index_sequence<>, std::index_sequence<>) {
      return {};
    }

    template <typename T, typename V>
    struct copy_referencenesss_
    {
      using type = T;
    };

    template <typename T, typename V>
    struct copy_referencenesss_<T,V&>
    {
      using type = T&;
    };

    template <typename T, typename V>
    struct copy_referencenesss_<T, V&&>
    {
      using type = std::remove_reference_t<T>&&;
    };

    template <typename T, typename V>
    using copy_referenceness = typename copy_referencenesss_<T,V>::type;

    template <typename T, typename TSource>
    using as_if_forwarded = std::conditional_t<
      !std::is_reference<TSource>{},
      std::add_rvalue_reference_t<std::remove_reference_t<T>>,
      copy_referenceness<T, TSource>
    >;

    template <typename TLike, typename T>
    constexpr
    decltype(auto)
    forward_like(T && x) noexcept
    {
      static_assert(!(std::is_rvalue_reference<decltype(x)>{} &&
                      std::is_lvalue_reference<TLike>{}));

      return static_cast<as_if_forwarded<T, TLike>>(x);
    }

    template<
      std::size_t I, std::size_t ... Is,
      std::size_t J, std::size_t ... Js
    >
    constexpr
    auto
    next_seq(std::index_sequence<I, Is...>, std::index_sequence<J, Js...>) {
      if constexpr (I + 1 == J) {
        return prepend<0>(next_seq(std::index_sequence<Is...>{},
                                   std::index_sequence<Js...>{}));
      } else {
        return std::index_sequence<I + 1, Is...>{};
      }
    }

    template<std::size_t ... I>
    static
    constexpr
    std::size_t
    sum(std::index_sequence<I...>) { return (I + ...); }

    template <typename T>
    using remove_cv_ref_t = std::remove_const_t<std::remove_reference_t<T>>;


    template <std::size_t I, typename T>
    decltype(auto) get(T&& t)
    {
      if constexpr (is_variant_v<T>)
      {
        return std::get<I>(std::forward<T>(t));
      }
      else
      {
        static_assert(I == 0);
        return std::forward<T>(t);
      }
    }

    template <std::size_t I, typename T>
    auto get_if(T* t)
    {
      if constexpr (is_variant_v<T>)
      {
        return std::get_if<I>(t);
      }
      else {
        static_assert(I == 0);
        return t;
      }
    }

    template <typename V>
    constexpr size_t variant_size()
    {
      if constexpr (is_variant_v<V>)
      {
        return std::variant_size_v<variant_access<V>>;
      }
      else
      {
        return 1;
      }
    }

    template <typename V>
    constexpr size_t index(const V& v)
    {
      if constexpr (is_variant_v<V>)
      {
        return v.index();
      }
      else
      {
        return 0;
      }
    }
    template<
      std::size_t ... Is,
      std::size_t ... Ms,
      typename F,
      typename ... Vs
    >
    inline
    constexpr
    auto
    visit(
      std::index_sequence<Is...> i,
      std::index_sequence<Ms...> m,
      F &&f,
      Vs &&... vs)
    {
      constexpr auto n = next_seq(i, m);
      if constexpr (sum(n) == 0) {
        return f(get<Is>(std::forward<Vs>(vs))...);
      } else {
        if (std::tuple(detail::index(vs)...) == std::tuple(Is...)) {
          return f(forward_like<Vs>(*get_if<Is>(&vs))...);
        }
        return visit(n, m, std::forward<F>(f), std::forward<Vs>(vs)...);
      }
    }

    template<typename>
    inline constexpr std::size_t zero = 0;
  }
  template <typename F, typename ... Vs>
  inline auto visit(F&& f, Vs&& ... vs)
  {
    if constexpr (((detail::variant_size<Vs>() == 1) && ...))
    {
      return f(detail::forward_like<Vs>(*detail::get_if<0>(&vs))...);
    } else {
      return detail::visit(
        std::index_sequence<detail::zero<Vs>...>{},
        std::index_sequence<detail::variant_size<Vs>()...>{},
        std::forward<F>(f),
        std::forward<Vs>(vs)...);
    }
  }

}

#endif //ROLLBEAR_VISIT_HPP
