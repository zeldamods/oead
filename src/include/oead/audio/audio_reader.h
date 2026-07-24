#pragma once

#include <nonstd/span.h>
#include <optional>
#include <type_traits>

#include "oead/audio/types.h"
#include "oead/util/align.h"
#include "oead/util/bit_utils.h"
#include "oead/util/swap.h"

namespace oead::util {
class AudioReader {
public:
  AudioReader() = default;
  AudioReader(tcb::span<const u8> data, Endianness endian) : m_data{data}, m_endian{endian} {}

  const auto& Span() const { return m_data; }
  size_t Tell() const { return m_offset; }
  void Seek(size_t offset) { m_offset = offset; }
  void SectionSeek(size_t offset) { m_offset = offset + m_section_offset; }
  void Align(std::size_t n) { Seek(AlignUp(Tell(), n)); }

  Endianness Endian() const { return m_endian; }
  void SetEndian(Endianness endian) { m_endian = endian; }

  template <typename T, bool Safe = true, 
            typename = std::enable_if_t<std::is_standard_layout<T>::value>>
  T Read() {
    if constexpr (Safe) {
      if (m_offset + sizeof(T) > m_data.size())
        throw std::out_of_range("Out of bounds read");
    }

    T value = BitCastPtr<T>(&m_data[m_offset]);
    SwapIfNeededInPlace(value, m_endian);
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

  void SwapEndianness() {
    m_endian = m_endian == util::Endianness::Little ? util::Endianness::Big : util::Endianness::Little;
  }

  void MarkSectionStart() { m_section_offset = Tell(); }
  size_t SectionStart() const { return m_section_offset; }

  audio::SoundFileHeader ReadSoundFileHeader() {
    std::size_t header_start {Tell()};

    audio::SoundFileHeader header;
    header.signature = Read<std::array<char, 4>>();
    
    header.byte_order_mark = Read<std::uint16_t>();
    if (util::ByteOrderMarkToEndianness(header.byte_order_mark) == util::Endianness::Little) {
      SwapEndianness();
      Seek(header_start);
      return ReadSoundFileHeader();
    }

    header.head_size = Read<std::uint16_t>();
    header.version = Read<std::uint32_t>();
    header.file_size = Read<std::uint32_t>();
    header.block_count = Read<std::uint16_t>();
    header.reserved = Read<std::uint16_t>();

    header.block_refs.resize(header.block_count);
    for (auto& block_ref : header.block_refs)
      block_ref = Read<audio::SizedReference>();

    return header;
  }

  template <typename T>
  audio::Table<T> ReadTable() {
    audio::Table<T> tbl;
    tbl.count = Read<std::uint32_t>();
    tbl.items.resize(tbl.count);
    for (auto& item : tbl.items)
      item = Read<T>();

    return tbl;
  }

  audio::Channel ReadSamples(uint total_samples, audio::SampleFormat format, bool is_wave) {
    uint sample_block_size {AlignUp(total_samples, 14) / 14 * 8};
    if (!is_wave)
      sample_block_size = AlignUp(sample_block_size, 0x20);

    audio::Channel channel;
    switch (format) {
    case audio::SampleFormat::PCMS8:
      channel.resize(sample_block_size);
      for (auto& sample : channel)
        sample = Read<std::int8_t>();
      break;
    case audio::SampleFormat::PCMS16:
      channel.resize(sample_block_size / sizeof(std::int16_t));
      for (auto& sample : channel)
        sample = Read<std::int16_t>();
      break;
    case audio::SampleFormat::DSPADPCM: {
      channel.resize(sample_block_size * 2);
      for (uint i {0}; i < channel.size(); i += 2) {
        auto sample_bytes = Read<std::uint8_t>();
        channel[i] = static_cast<std::uint8_t>(sample_bytes & 0b1111);
        channel[i + 1] = static_cast<std::uint8_t>(sample_bytes >> 4);
      }
      break;
    }
    case audio::SampleFormat::PCMS32:
      channel.resize(sample_block_size / sizeof(std::int32_t));
      for (auto& sample : channel)
        sample = Read<std::int32_t>();
      break;
    }
    
    return channel;
  }

private:
  tcb::span<const u8> m_data{};
  size_t m_offset {0};
  size_t m_section_offset {0};
  Endianness m_endian = Endianness::Big;
};
} // namespace oead::util