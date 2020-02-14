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

#include <memory>
#include <type_traits>

namespace oead::util {

template <class... T>
struct AlwaysFalse : std::false_type {};

template <class T>
struct IsUniquePtr : std::false_type {};

template <class T, class D>
struct IsUniquePtr<std::unique_ptr<T, D>> : std::true_type {};

template <class T, class...>
struct IsAnyOfType : std::false_type {};
template <class T, class Head, class... Tail>
struct IsAnyOfType<T, Head, Tail...>
    : std::conditional_t<std::is_same_v<T, Head>, std::true_type, IsAnyOfType<T, Tail...>> {};

template <typename T>
constexpr T& AsMutable(T const& value) noexcept {
  return const_cast<T&>(value);
}
template <typename T>
constexpr T* AsMutable(T const* value) noexcept {
  return const_cast<T*>(value);
}
template <typename T>
constexpr T* AsMutable(T* value) noexcept {
  return value;
}
template <typename T>
void AsMutable(T const&&) = delete;

}  // namespace oead::util
