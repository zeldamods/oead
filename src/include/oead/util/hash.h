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

#include <string_view>

#include <oead/types.h>

namespace oead::util {

template <typename CharType = u8>
constexpr u32 crc32(const CharType* data, std::size_t size) {
  u32 crc = 0xFFFFFFFF;
  for (std::size_t i = 0; i < size; ++i) {
    crc ^= u8(data[i]);
    for (std::size_t j = 0; j < 8; ++j) {
      u32 mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
  }
  return ~crc;
}

constexpr u32 crc32(std::string_view str) {
  return crc32<char>(str.data(), str.size());
}

}  // namespace oead::util
