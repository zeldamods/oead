/**
 * Copyright (C) 2020 leoetlino
 *
 * This file is part of oead.
 *
 * oead is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * oead is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oead.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <nonstd/visit.h>
#include <type_traits>

#include <oead/util/type_utils.h>

namespace oead::util {

template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...)->Overloaded<Ts...>;

/// Helper function to visit a std::variant efficiently.
template <typename Visitor, typename... Variants>
constexpr auto Visit(Visitor&& visitor, Variants&&... variants) {
  static_assert((rollbear::detail::is_variant_v<Variants> && ...), "need variants");
  return rollbear::visit(
      [&visitor](auto&& value) {
        using T = decltype(value);
        if constexpr (util::IsUniquePtr<std::decay_t<T>>())
          return visitor(std::forward<typename std::decay_t<T>::element_type>(*value));
        else
          return visitor(std::forward<T>(value));
      },
      std::forward<Variants>(variants)...);
}

/// Helper function to visit a std::variant efficiently. More convenient to use in most cases.
template <typename Variant, typename... Ts>
constexpr auto Match(Variant&& variant, Ts&&... lambdas) {
  return Visit(Overloaded{std::forward<Ts>(lambdas)...}, std::forward<Variant>(variant));
}

/// A std::variant wrapper that transparently dereferences unique_ptrs. This is intended for be
/// used for variants that can contain possibly large values.
template <typename EnumType, typename... Types>
struct Variant {
  using Storage = std::variant<Types...>;

  Variant() = default;
  Variant(const Variant& other) { *this = other; }
  Variant(Variant&& other) noexcept { *this = std::move(other); }

  template <typename T,
            std::enable_if_t<IsAnyOfType<std::decay_t<T>, Types...>() ||
                             IsAnyOfType<std::unique_ptr<std::decay_t<T>>, Types...>()>* = nullptr>
  Variant(const T& value) {
    if constexpr (IsAnyOfType<std::unique_ptr<std::decay_t<T>>, Types...>())
      v = std::make_unique<T>(value);
    else
      v = value;
  }

  template <typename T,
            std::enable_if_t<IsAnyOfType<std::decay_t<T>, Types...>() ||
                             IsAnyOfType<std::unique_ptr<std::decay_t<T>>, Types...>()>* = nullptr>
  Variant(T&& value) noexcept {
    if constexpr (IsAnyOfType<std::unique_ptr<std::decay_t<T>>, Types...>())
      v = std::make_unique<T>(std::move(value));
    else
      v = std::move(value);
  }

  Variant& operator=(const Variant& other) {
    Visit([this](const auto& value) { *this = value; }, other.v);
    return *this;
  }

  Variant& operator=(Variant&& other) noexcept {
    v = std::move(other.v);
    return *this;
  }

  template <typename H>
  friend H AbslHashValue(H h, const Variant& self) {
    return Visit([&](const auto& value) { return H::combine(std::move(h), self.v.index(), value); },
                 self.v);
  }

  EnumType GetType() const { return EnumType(v.index()); }

  template <EnumType type>
  const auto& Get() const {
    using T = std::variant_alternative_t<static_cast<size_t>(type), Storage>;
    if constexpr (util::IsUniquePtr<T>())
      return *std::get<T>(v);
    else
      return std::get<T>(v);
  }

  template <EnumType type>
  auto& Get() {
    using T = std::variant_alternative_t<static_cast<size_t>(type), Storage>;
    if constexpr (util::IsUniquePtr<T>())
      return *std::get<T>(v);
    else
      return std::get<T>(v);
  }

  Storage v;
};

template <typename... Types>
inline bool operator==(const Variant<Types...>& lhs, const Variant<Types...>& rhs) {
  if (lhs.v.index() != rhs.v.index())
    return false;
  return rollbear::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (util::IsUniquePtr<T>()) {
          const auto& other = std::get<T>(lhs.v);
          return other == value || *other == *value;
        } else {
          return std::get<T>(lhs.v) == value;
        }
      },
      rhs.v);
}

}  // namespace oead::util
