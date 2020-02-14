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
#include <nonstd/span.h>
#include <optional>
#include <vector>

#include <oead/types.h>
#include <oead/util/swap.h>

namespace oead::yaz0 {

struct Header {
  /// 'Yaz0'
  std::array<char, 4> magic;
  /// Size of uncompressed data
  util::BeInt<u32> uncompressed_size;
  /// [Newer files only] Required buffer alignment
  util::BeInt<u32> data_alignment;
  /// Unused (as of December 2019)
  std::array<u8, 4> reserved;
};
static_assert(sizeof(Header) == 0x10);

std::optional<Header> GetHeader(tcb::span<const u8> data);

/// @param src  Source data
/// @param data_alignment  Required buffer alignment hint for decompression
/// @param level  Compression level (6 to 9; 6 is fastest and 9 is slowest)
std::vector<u8> Compress(tcb::span<const u8> src, u32 data_alignment = 0, int level = 7);

std::vector<u8> Decompress(tcb::span<const u8> src);
/// For increased flexibility, allocating the destination buffer can be done manually.
/// In that case, the header is assumed to be valid, and the buffer size
/// must be equal to the uncompressed data size.
void Decompress(tcb::span<const u8> src, tcb::span<u8> dst);
/// Same, but additionally assumes that the source is well-formed.
/// DO NOT USE THIS FOR UNTRUSTED SOURCES.
void DecompressUnsafe(tcb::span<const u8> src, tcb::span<u8> dst);

}  // namespace oead::yaz0
