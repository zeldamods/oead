/**
 * Copyright (C) 2019 leoetlino <leo@leolam.fr>
 *
 * This file is part of syaz0.
 *
 * syaz0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * syaz0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with syaz0.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <cstring>
#include <nonstd/span.h>
#include <optional>
#include <type_traits>
#include <vector>

#include <oead/types.h>
#include <oead/util/align.h>
#include <oead/util/bit_utils.h>
#include <oead/util/swap.h>

namespace oead::util {

/// A simple binary data reader that automatically byteswaps and avoids undefined behaviour.
class BinaryReader final {
public:
  BinaryReader() = default;
  BinaryReader(tcb::span<const u8> data, Endianness endian) : m_data{data}, m_endian{endian} {}

  const auto& span() const { return m_data; }
  size_t Tell() const { return m_offset; }
  void Seek(size_t offset) { m_offset = offset; }

  Endianness Endian() const { return m_endian; }

  template <typename T, bool Safe = true>
  std::optional<T> Read(std::optional<size_t> offset = std::nullopt) {
    if (offset)
      Seek(*offset);
    static_assert(std::is_standard_layout<T>());
    if constexpr (Safe) {
      if (m_offset + sizeof(T) > m_data.size())
        return std::nullopt;
    }
    T value = util::BitCastPtr<T>(&m_data[m_offset]);
    util::SwapIfNeededInPlace(value, m_endian);
    m_offset += sizeof(T);
    return value;
  }

  template <bool Safe = true>
  std::optional<u32> ReadU24(std::optional<size_t> read_offset = std::nullopt) {
    if (read_offset)
      Seek(*read_offset);
    if constexpr (Safe) {
      if (m_offset + 3 > m_data.size())
        return std::nullopt;
    }
    const size_t offset = m_offset;
    m_offset += 3;
    if (m_endian == Endianness::Big)
      return m_data[offset] << 16 | m_data[offset + 1] << 8 | m_data[offset + 2];
    return m_data[offset + 2] << 16 | m_data[offset + 1] << 8 | m_data[offset];
  }

  template <typename StringType = std::string>
  StringType ReadString(size_t offset, std::optional<size_t> max_len = std::nullopt) const {
    if (offset > m_data.size())
      throw std::out_of_range("Out of bounds string read");

    // Ensure strnlen doesn't go out of bounds.
    if (!max_len || *max_len > m_data.size() - offset)
      max_len = m_data.size() - offset;

    const char* ptr = reinterpret_cast<const char*>(&m_data[offset]);
    return {ptr, strnlen(ptr, *max_len)};
  }

private:
  tcb::span<const u8> m_data{};
  size_t m_offset = 0;
  Endianness m_endian = Endianness::Big;
};

template <typename Storage>
class BinaryWriterBase {
public:
  BinaryWriterBase(Endianness endian) : m_endian{endian} {}

  /// Returns a std::vector<u8> with everything written so far, and resets the buffer.
  std::vector<u8> Finalize() { return std::move(m_data); }

  const auto& Buffer() const { return m_data; }
  auto& Buffer() { return m_data; }
  size_t Tell() const { return m_offset; }
  void Seek(size_t offset) { m_offset = offset; }

  Endianness Endian() const { return m_endian; }
  BinaryReader Reader() const { return {m_data, m_endian}; }

  void WriteBytes(tcb::span<const u8> bytes) {
    if (m_offset + bytes.size() > m_data.size())
      m_data.resize(m_offset + bytes.size());

    std::memcpy(&m_data[m_offset], bytes.data(), bytes.size());
    m_offset += bytes.size();
  }

  template <typename T, typename std::enable_if_t<!std::is_pointer_v<T> &&
                                                  std::is_trivially_copyable_v<T>>* = nullptr>
  void Write(T value) {
    SwapIfNeededInPlace(value, m_endian);
    WriteBytes({reinterpret_cast<const u8*>(&value), sizeof(value)});
  }

  void Write(std::string_view str) {
    WriteBytes({reinterpret_cast<const u8*>(str.data()), str.size()});
  }
  void WriteCStr(std::string_view str) {
    Write(str);
    WriteNul();
  }

  void WriteU24(u32 value) {
    if (m_endian == Endianness::Big)
      Write<U24<true>>(value);
    else
      Write<U24<false>>(value);
  }

  void WriteNul() { Write<u8>(0); }

  template <typename Callable>
  void RunAt(size_t offset, Callable fn) {
    const size_t current_offset = Tell();
    Seek(offset);
    fn(current_offset);
    Seek(current_offset);
  }

  template <typename T>
  void WriteCurrentOffsetAt(size_t offset, size_t base = 0) {
    RunAt(offset, [this, base](size_t current_offset) { Write(T(current_offset - base)); });
  }

  void AlignUp(size_t n) { Seek(util::AlignUp(Tell(), n)); }
  void GrowBuffer() {
    if (m_offset > m_data.size())
      m_data.resize(m_offset);
  }

private:
  Storage m_data;
  size_t m_offset = 0;
  Endianness m_endian;
};

using BinaryWriter = BinaryWriterBase<std::vector<u8>>;

}  // namespace oead::util
