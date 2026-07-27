#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "oead/audio/element_type.h"
#include "oead/audio/types.h"
#include "oead/util/binary_reader.h"

namespace oead::util {
class AudioWriter : public BinaryWriter {
public:
  using BinaryWriter::BinaryWriterBase;

  void WriteString(std::string_view str) {
    WriteBytes({reinterpret_cast<const u8*>(str.data()), str.size()});
  }
  void WriteCString(std::string_view str) {
    WriteString(str);
    WriteNul();
  }

  std::map<std::string, std::vector<std::size_t>> WriteSoundFileHeader(std::array<char, 4> signature, 
                                                                       int version, 
                                                                       std::vector<audio::ElementType> block_ids) {
    std::size_t header_start {Tell()};
    std::map<std::string, std::vector<std::size_t>> pending_values;

    Write(signature);
    Write<std::uint16_t>(0xFEFF);
    std::size_t size_pos = WritePendingValue<std::uint16_t>();
    Write<std::uint32_t>(version);
    pending_values["file_size"] = {WritePendingValue()};

    auto invalid_count {std::count(block_ids.begin(), block_ids.end(), audio::ElementType::Invalid)};

    Write<std::uint16_t>(block_ids.size() - invalid_count);
    Write<std::uint16_t>(0);

    pending_values["blocks"] = std::vector<std::size_t>(block_ids.size());
    pending_values["blocks_size"] = std::vector<std::size_t>(block_ids.size());
    for (uint i {0}; i < pending_values["blocks"].size(); ++i) {
      if (block_ids[i] != audio::ElementType::Invalid) {
        pending_values["blocks"][i] = WriteEmptyOffsetReference(block_ids[i], true, true);
        pending_values["blocks_size"][i] = pending_values["blocks"][i] + sizeof(std::int32_t);
      }
    }

    AlignUp(0x20);

    WriteCurrentOffsetAt<std::uint16_t>(size_pos, header_start);

    return pending_values;
  }

  std::size_t WriteEmptyOffsetReference(audio::ElementType reference_type, 
                                        bool valid_ref,
                                        bool with_size = false) {
    if (valid_ref) {
      Write(static_cast<std::uint16_t>(reference_type));
      Write<std::uint16_t>(0);
  
      std::size_t offset_pos = Tell();
      Write<std::uint32_t>(0);
  
      if (with_size)
        Write<std::uint32_t>(0);

      return offset_pos;
    }

    Write<std::uint32_t>(0);
    Write<std::int32_t>(-1);
    if (with_size)
      Write<std::uint32_t>(0);

    return 0;
  }

template <typename ValueType = std::int32_t>
  std::size_t WritePendingValue() {
    std::size_t value_offset = Tell();
    Write<ValueType>(0);
    return value_offset;
  }

  void WriteSamples(const audio::Channel& channel, audio::SampleFormat format) {
    switch (format) {
    case audio::SampleFormat::PCMS8:
      for (auto& sample : channel)
        Write(std::get<std::int8_t>(sample));
      break;
    case audio::SampleFormat::PCMS16:
      for (auto& sample : channel)
        Write(std::get<std::int16_t>(sample));
      break;
    case audio::SampleFormat::DSPADPCM:
      for (uint i {0}; i < channel.size(); i += 2) {
        std::uint8_t sample_byte_lo {std::get<std::uint8_t>(channel[i])};
        std::uint8_t sample_byte_hi {std::get<std::uint8_t>(channel[i + 1])};
          
        std::uint8_t sample_byte = (sample_byte_hi << 4) | sample_byte_lo;
        Write(sample_byte);
      }
      break;
    case audio::SampleFormat::PCMS32:
      for (auto& sample : channel)
        Write(std::get<std::int32_t>(sample));
      break;
    }
  }
};
} // namespace oead::util