// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <concepts>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace oead::util {

template <typename T, typename PtrType>
class BitCastPtrType {
public:
  static_assert(std::is_trivially_copyable<PtrType>(),
                "BitCastPtr source type must be trivially copyable.");
  static_assert(std::is_trivially_copyable<T>(),
                "BitCastPtr destination type must be trivially copyable.");

  explicit BitCastPtrType(PtrType* ptr) : m_ptr(ptr) {}

  // Enable operator= only for pointers to non-const data
  template <std::same_as<T> S>
    requires(!std::is_const_v<PtrType>)
  inline void operator=(const S& source) {
    std::memcpy(m_ptr, &source, sizeof(source));
  }

  inline operator T() const {
    T result;
    std::memcpy(&result, m_ptr, sizeof(result));
    return result;
  }

private:
  PtrType* m_ptr;
};

// Provides an aliasing-safe alternative to reinterpret_cast'ing pointers to structs
// Conversion constructor and operator= provided for a convenient syntax.
// Usage: MyStruct s = BitCastPtr<MyStruct>(some_ptr);
// BitCastPtr<MyStruct>(some_ptr) = s;
template <typename T, typename PtrType>
inline auto BitCastPtr(PtrType* ptr) noexcept -> BitCastPtrType<T, PtrType> {
  return BitCastPtrType<T, PtrType>{ptr};
}

template <typename T>
class FlagBit {
public:
  FlagBit(std::underlying_type_t<T>& bits, T bit) : m_bits(bits), m_bit(bit) {}
  explicit operator bool() const { return (m_bits & std::to_underlying(m_bit)) != 0; }
  FlagBit& operator=(const bool rhs) {
    if (rhs)
      m_bits |= std::to_underlying(m_bit);
    else
      m_bits &= ~std::to_underlying(m_bit);
    return *this;
  }

private:
  std::underlying_type_t<T>& m_bits;
  T m_bit;
};

template <typename T>
class Flags {
public:
  constexpr Flags() = default;
  constexpr Flags(std::initializer_list<T> bits) {
    for (auto bit : bits) {
      m_hex |= std::to_underlying(bit);
    }
  }
  FlagBit<T> operator[](T bit) { return FlagBit(m_hex, bit); }
  constexpr bool operator[](T bit) const { return (m_hex & std::to_underlying(bit)) != 0; }
  constexpr friend bool operator==(Flags a, Flags b) { return a.m_hex == b.m_hex; }

  std::underlying_type_t<T> m_hex = 0;
};

}  // namespace oead::util
