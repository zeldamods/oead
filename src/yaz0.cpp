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

#include <algorithm>
#include <bitset>
#include <cstring>

#include <zlib-ng.h>

#include <oead/util/binary_reader.h>
#include <oead/yaz0.h>

namespace oead::yaz0 {

constexpr std::array<char, 4> Magic = {'Y', 'a', 'z', '0'};
constexpr size_t ChunksPerGroup = 8;
constexpr size_t MaximumMatchLength = 0xFF + 0x12;
constexpr size_t WindowSize = 0x1000;

static std::optional<Header> GetHeader(util::BinaryReader& reader) {
  const auto header = reader.Read<Header>();
  if (!header)
    return std::nullopt;
  if (header->magic != Magic)
    return std::nullopt;
  return header;
}

std::optional<Header> GetHeader(tcb::span<const u8> data) {
  util::BinaryReader reader{data, util::Endianness::Big};
  return GetHeader(reader);
}

namespace {
class GroupWriter {
public:
  GroupWriter(std::vector<u8>& result) : m_result{result} { Reset(); }

  void HandleZlibMatch(u32 dist, u32 lc) {
    if (dist == 0) {
      // Literal.
      m_group_header.set(7 - m_pending_chunks);
      m_result.push_back(u8(lc));
    } else {
      // Back reference.
      constexpr u32 ZlibMinMatch = 3;
      WriteMatch(dist - 1, lc + ZlibMinMatch);
    }

    ++m_pending_chunks;
    if (m_pending_chunks == ChunksPerGroup) {
      m_result[m_group_header_offset] = u8(m_group_header.to_ulong());
      Reset();
    }
  }

  // Must be called after zlib has completed to ensure the last group is written.
  void Finalise() {
    if (m_pending_chunks != 0)
      m_result[m_group_header_offset] = u8(m_group_header.to_ulong());
  }

private:
  void Reset() {
    m_pending_chunks = 0;
    m_group_header.reset();
    m_group_header_offset = m_result.size();
    m_result.push_back(0xFF);
  }

  void WriteMatch(u32 distance, u32 length) {
    if (length < 18) {
      m_result.push_back(((length - 2) << 4) | u8(distance >> 8));
      m_result.push_back(u8(distance));
    } else {
      // If the match is longer than 18 bytes, 3 bytes are needed to write the match.
      const size_t actual_length = std::min<size_t>(MaximumMatchLength, length);
      m_result.push_back(u8(distance >> 8));
      m_result.push_back(u8(distance));
      m_result.push_back(u8(actual_length - 0x12));
    }
  }

  std::vector<u8>& m_result;
  size_t m_pending_chunks;
  std::bitset<8> m_group_header;
  std::size_t m_group_header_offset;
};
}  // namespace

std::vector<u8> Compress(tcb::span<const u8> src, u32 data_alignment, int level) {
  std::vector<u8> result(sizeof(Header));
  result.reserve(src.size());

  // Write the header.
  Header header;
  header.magic = Magic;
  header.uncompressed_size = u32(src.size());
  header.data_alignment = data_alignment;
  header.reserved.fill(0);
  std::memcpy(result.data(), &header, sizeof(header));

  GroupWriter writer{result};

  // Let zlib do the heavy lifting.
  std::array<u8, 8> dummy{};
  size_t dummy_size = dummy.size();
  const int ret = zng_compress2(
      dummy.data(), &dummy_size, src.data(), src.size(), std::clamp<int>(level, 6, 9),
      [](void* w, u32 dist, u32 lc) { static_cast<GroupWriter*>(w)->HandleZlibMatch(dist, lc); },
      &writer);
  if (ret != Z_OK)
    throw std::runtime_error("zng_compress failed");

  writer.Finalise();
  return result;
}

std::vector<u8> Decompress(tcb::span<const u8> src) {
  const auto header = GetHeader(src);
  if (!header)
    return {};
  std::vector<u8> result(header->uncompressed_size);
  Decompress(src, result);
  return result;
}

template <bool Safe>
static void Decompress(tcb::span<const u8> src, tcb::span<u8> dst) {
  util::BinaryReader reader{src, util::Endianness::Big};
  reader.Seek(sizeof(Header));

  u8 group_header = 0;
  size_t remaining_chunks = 0;
  for (auto dst_it = dst.begin(); dst_it < dst.end();) {
    if (remaining_chunks == 0) {
      group_header = reader.Read<u8, Safe>().value();
      remaining_chunks = ChunksPerGroup;
    }

    if (group_header & 0x80) {
      *dst_it++ = reader.Read<u8, Safe>().value();
    } else {
      const u16 pair = reader.Read<u16, Safe>().value();
      const size_t distance = (pair & 0x0FFF) + 1;
      const size_t length =
          ((pair >> 12) ? (pair >> 12) : (reader.Read<u8, Safe>().value() + 16)) + 2;

      const u8* base = dst_it - distance;
      if (base < dst.begin() || dst_it + length > dst.end()) {
        throw std::invalid_argument("Copy is out of bounds");
      }
#pragma GCC unroll 0
      for (size_t i = 0; i < length; ++i)
        *dst_it++ = base[i];
    }

    group_header <<= 1;
    remaining_chunks -= 1;
  }
}

void Decompress(tcb::span<const u8> src, tcb::span<u8> dst) {
  Decompress<true>(src, dst);
}

void DecompressUnsafe(tcb::span<const u8> src, tcb::span<u8> dst) {
  Decompress<false>(src, dst);
}

}  // namespace oead::yaz0
