// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <bit>
#include <tuple>
#include <type_traits>

#include <oead/types.h>
#include <oead/util/type_utils.h>

namespace oead::util {

enum class Endianness {
  Big,
  Little,
};

namespace detail {
constexpr Endianness PlatformEndianness =
    std::endian::native == std::endian::big ? Endianness::Big : Endianness::Little;
}  // namespace detail

/// Byte swap a value.
template <typename T>
constexpr T SwapValue(T data) {
  static_assert(std::is_arithmetic<T>(), "function only makes sense with arithmetic types");

  if constexpr (std::is_integral<T>()) {
    return std::byteswap(data);
  } else {
    using Bits = std::conditional_t<sizeof(T) == sizeof(u32), u32, u64>;
    return std::bit_cast<T>(std::byteswap(std::bit_cast<Bits>(data)));
  }
}

/// Swap a value if its endianness is not the same as the machine endianness.
/// @param endian  The endianness of the value.
template <typename T>
void SwapIfNeededInPlace(T& value, Endianness endian) {
  if (detail::PlatformEndianness == endian)
    return;

  if constexpr (std::is_arithmetic<T>()) {
    value = SwapValue(value);
  }

  if constexpr (util::ExposesFields<T>()) {
    std::apply([endian](auto&... fields) { (SwapIfNeededInPlace(fields, endian), ...); },
               value.fields());
  }
}

template <typename T>
T SwapIfNeeded(T value, Endianness endian) {
  SwapIfNeededInPlace(value, endian);
  return value;
}

constexpr Endianness ByteOrderMarkToEndianness(u16 bom) {
  return bom == 0xFEFF ? Endianness::Big : Endianness::Little;
}

/// A wrapper that stores an integer in the specified endianness and automatically bytes swap when
/// reading/writing the value.
template <typename T, Endianness Endian>
struct EndianInt {
  static_assert(std::is_arithmetic<T>(), "T must be an arithmetic type");
  EndianInt() = default;
  EndianInt(T val) { *this = val; }
  operator T() const { return SwapIfNeeded(raw, Endian); }
  EndianInt& operator=(T v) {
    raw = SwapIfNeeded(v, Endian);
    return *this;
  }

private:
  T raw;
};

template <typename T>
using BeInt = EndianInt<T, Endianness::Big>;
template <typename T>
using LeInt = EndianInt<T, Endianness::Little>;

}  // namespace oead::util

namespace oead {
template <typename T, util::Endianness Endian>
struct NumberType<util::EndianInt<T, Endian>> {
  using type = T;
};
}  // namespace oead
