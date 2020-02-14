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
#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using size_t = std::size_t;

using f32 = float;
using f64 = double;

namespace oead {

#define OEAD_DEFINE_FIELDS(TYPE, ...)                                                              \
  constexpr auto fields() { return std::tie(__VA_ARGS__); }                                        \
  constexpr auto fields() const { return std::tie(__VA_ARGS__); }                                  \
  constexpr friend bool operator==(const TYPE& lhs, const TYPE& rhs) {                             \
    return lhs.fields() == rhs.fields();                                                           \
  }                                                                                                \
  constexpr friend bool operator!=(const TYPE& lhs, const TYPE& rhs) { return !(lhs == rhs); }     \
  template <typename H>                                                                            \
  friend H AbslHashValue(H h, const TYPE& self) {                                                  \
    return H::combine(std::move(h), self.fields());                                                \
  }

/// Strongly typed wrapper around arithmetic types
/// to make types clear especially for Python bindings.
template <typename T>
struct Number {
  static_assert(std::is_arithmetic<T>(), "T must be an arithmetic type");
  constexpr Number() = default;
  constexpr explicit Number(T v) : value{v} {}
  constexpr operator T() const { return value; }
  constexpr Number& operator=(T v) { return value = v, *this; }
  constexpr Number& operator++(int) { return ++value, *this; }
  constexpr Number& operator--(int) { return --value, *this; }
  constexpr Number& operator++() { return value++, *this; }
  constexpr Number& operator--() { return value--, *this; }
  constexpr Number& operator+=(T rhs) { return value += rhs, *this; }
  constexpr Number& operator-=(T rhs) { return value -= rhs, *this; }
  constexpr Number& operator*=(T rhs) { return value *= rhs, *this; }
  constexpr Number& operator/=(T rhs) { return value /= rhs, *this; }
  constexpr Number& operator%=(T rhs) { return value %= rhs, *this; }
  constexpr Number& operator&=(T rhs) { return value &= rhs, *this; }
  constexpr Number& operator|=(T rhs) { return value |= rhs, *this; }
  constexpr Number& operator<<=(T rhs) { return value <<= rhs, *this; }
  constexpr Number& operator>>=(T rhs) { return value >>= rhs, *this; }
  OEAD_DEFINE_FIELDS(Number, value);

  T value;
};

using U8 = Number<std::uint8_t>;
using U16 = Number<std::uint16_t>;
using U32 = Number<std::uint32_t>;
using U64 = Number<std::uint64_t>;

using S8 = Number<std::int8_t>;
using S16 = Number<std::int16_t>;
using S32 = Number<std::int32_t>;
using S64 = Number<std::int64_t>;

using F32 = Number<float>;
using F64 = Number<double>;

/// Unsigned 24-bit integer.
template <bool BigEndian>
struct U24 {
  constexpr U24() = default;
  constexpr U24(u32 v) { Set(v); }
  constexpr operator u32() const { return Get(); }
  constexpr U24& operator=(u32 v) { return Set(v), *this; }

private:
  constexpr u32 Get() const {
    if constexpr (BigEndian)
      return data[0] << 16 | data[1] << 8 | data[2];
    else
      return data[2] << 16 | data[1] << 8 | data[0];
  }

  constexpr void Set(u32 v) {
    if constexpr (BigEndian) {
      data[0] = (v >> 16) & 0xFF;
      data[1] = (v >> 8) & 0xFF;
      data[2] = v & 0xFF;
    } else {
      data[2] = (v >> 16) & 0xFF;
      data[1] = (v >> 8) & 0xFF;
      data[0] = v & 0xFF;
    }
  }

  std::array<u8, 3> data;
};
static_assert(sizeof(U24<false>) == 3);

/// Underlying type for an arithmetic type.
template <typename T>
struct NumberType {
  static_assert(std::is_arithmetic<T>(), "Only sane for arithmetic types!");
  using type = T;
};

/// Underlying type for an arithmetic type.
template <typename T>
struct NumberType<Number<T>> {
  using type = T;
};

/// 2D vector.
template <typename T>
struct Vector2 {
  T x, y;
  OEAD_DEFINE_FIELDS(Vector2, x, y);
};

/// 3D vector.
template <typename T>
struct Vector3 {
  T x, y, z;
  OEAD_DEFINE_FIELDS(Vector3, x, y, z);
};

/// 4D vector.
template <typename T>
struct Vector4 {
  T x, y, z, t;
  OEAD_DEFINE_FIELDS(Vector4, x, y, z, t);
};

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

/// Quaternion.
template <typename T>
struct Quat {
  T a, b, c, d;
  OEAD_DEFINE_FIELDS(Quat, a, b, c, d);
};
using Quatf = Quat<float>;

/// RGBA color (Red/Green/Blue/Alpha).
struct Color4f {
  float r, g, b, a;
  OEAD_DEFINE_FIELDS(Color4f, r, g, b, a);
};

/// Curve (sead::hostio::curve*)
struct Curve {
  u32 a, b;
  std::array<float, 30> floats;
  OEAD_DEFINE_FIELDS(Curve, a, b, floats);
};
static_assert(sizeof(Curve) == 0x80);

/// A string class with its own inline, fixed-size storage.
template <size_t N>
struct FixedSafeString {
  FixedSafeString() = default;
  FixedSafeString(std::string_view str) { *this = str; }

  auto& operator=(const FixedSafeString& other) {
    length = other.length;
    data = other.data;
    return *this;
  }
  auto& operator=(std::string_view str) {
    length = std::min(str.size(), N);
    std::copy_n(str.begin(), length, data.begin());
    return *this;
  }

  operator std::string_view() const { return {data.data(), length}; }
  bool operator==(const FixedSafeString& other) const { return Str(*this) == Str(other); }
  bool operator!=(const FixedSafeString& other) const { return !(*this == other); }

private:
  size_t length = 0;
  std::array<char, N> data;
};

/// Casts a string-like object to a string view.
template <typename T>
std::string_view Str(const T& str_like) {
  return static_cast<std::string_view>(str_like);
}

}  // namespace oead
