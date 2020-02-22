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

#include <algorithm>
#include <tuple>

namespace oead::util {

template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto Enumerate(T&& iterable) {
  // Source: http://reedbeta.com/blog/python-like-enumerate-in-cpp17/
  struct iterator {
    size_t i;
    TIter iter;
    bool operator!=(const iterator& other) const { return iter != other.iter; }
    void operator++() {
      ++i;
      ++iter;
    }
    auto operator*() const { return std::tie(i, *iter); }
  };
  struct iterable_wrapper {
    T iterable;
    auto begin() { return iterator{0, std::begin(iterable)}; }
    auto end() { return iterator{0, std::end(iterable)}; }
  };
  return iterable_wrapper{std::forward<T>(iterable)};
}

template <typename First, typename... T>
constexpr inline bool IsAnyOf(const First& first, const T&... t) {
  return ((first == t) || ...);
}

template <typename T, typename Iterator>
constexpr bool Contains(const Iterator& begin, const Iterator& end, const T& value) {
  return std::find(begin, end, value) != end;
}

template <typename T, typename Container>
constexpr bool Contains(const Container& container, const T& value) {
  return std::find(std::begin(container), std::end(container), value) != std::end(container);
}

}  // namespace oead::util
