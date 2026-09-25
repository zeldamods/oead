#pragma once

#include "oead/audio/types.h"
#include "oead/util/binary_reader.h"

namespace oead::util {
class AudioReader : public BinaryReader {
public:
  using BinaryReader::BinaryReader;

  void SectionSeek(size_t offset) { Seek(offset + m_section_offset); }
  void Align(std::size_t n) { Seek(AlignUp(Tell(), n)); }

  void SwapEndianness() {
    SetEndian(Endian() == util::Endianness::Little ? util::Endianness::Big :
                                                     util::Endianness::Little);
  }

  void MarkSectionStart() { m_section_offset = Tell(); }
  size_t SectionStart() const { return m_section_offset; }

  audio::SoundFileHeader ReadSoundFileHeader() {
    std::size_t header_start{Tell()};

    audio::SoundFileHeader header;
    header.signature = *Read<std::array<char, 4>>();

    header.byte_order_mark = *Read<u16>();
    if (util::ByteOrderMarkToEndianness(header.byte_order_mark) == util::Endianness::Little) {
      SwapEndianness();
      Seek(header_start);
      return ReadSoundFileHeader();
    }

    header.head_size = *Read<u16>();
    header.version = *Read<u32>();
    header.file_size = *Read<u32>();
    header.block_count = *Read<u16>();
    header.reserved = *Read<u16>();

    header.block_refs.resize(header.block_count);
    for (auto& block_ref : header.block_refs)
      block_ref = *Read<audio::SizedReference>();

    return header;
  }

  template <typename T>
  audio::Table<T> ReadTable() {
    audio::Table<T> tbl;
    tbl.count = *Read<u32>();
    tbl.items.resize(tbl.count);
    for (auto& item : tbl.items)
      item = *Read<T>();

    return tbl;
  }

  audio::Channel ReadSamples(u32 total_samples, audio::SampleFormat format, bool is_wave) {
    u32 sample_block_size{AlignUp(total_samples, 14) / 14 * 8};
    if (!is_wave)
      sample_block_size = AlignUp(sample_block_size, 0x20);

    audio::Channel channel;
    switch (format) {
    case audio::SampleFormat::PCMS8:
      channel.resize(sample_block_size);
      for (auto& sample : channel)
        sample = *Read<s8>();
      break;
    case audio::SampleFormat::PCMS16:
      channel.resize(sample_block_size / sizeof(s16));
      for (auto& sample : channel)
        sample = *Read<s16>();
      break;
    case audio::SampleFormat::DSPADPCM: {
      channel.resize(sample_block_size);
      for (auto& sample : channel)
        sample = *Read<u8>();
      break;
    }
    case audio::SampleFormat::PCMS32:
      channel.resize(sample_block_size / sizeof(s32));
      for (auto& sample : channel)
        sample = *Read<s32>();
      break;
    }

    return channel;
  }

private:
  size_t m_section_offset{0};
};
}  // namespace oead::util
