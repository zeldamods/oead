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

  auto WriteSoundFileHeader(std::array<char, 4> signature, int version,
                            std::vector<audio::ElementType> block_ids) {
    std::size_t header_start{Tell()};
    std::map<std::string, std::vector<std::size_t>> pending_values;

    Write(signature);
    Write<u16>(0xFEFF);
    std::size_t size_pos = WritePendingValue<u16>();
    Write<u32>(version);
    pending_values["file_size"] = {WritePendingValue()};

    auto invalid_count{std::count(block_ids.begin(), block_ids.end(), audio::ElementType::Invalid)};

    Write<u16>(block_ids.size() - invalid_count);
    Write<u16>(0);

    pending_values["blocks"] = std::vector<std::size_t>(block_ids.size());
    pending_values["blocks_size"] = std::vector<std::size_t>(block_ids.size());
    for (u32 i{0}; i < pending_values["blocks"].size(); ++i) {
      if (block_ids[i] != audio::ElementType::Invalid) {
        pending_values["blocks"][i] = WriteEmptyOffsetReference(block_ids[i], true, true);
        pending_values["blocks_size"][i] = pending_values["blocks"][i] + sizeof(s32);
      }
    }

    AlignUp(0x20);

    WriteCurrentOffsetAt<u16>(size_pos, header_start);

    return pending_values;
  }

  std::size_t WriteEmptyOffsetReference(audio::ElementType reference_type, bool valid_ref,
                                        bool with_size = false) {
    if (valid_ref) {
      Write(static_cast<u16>(reference_type));
      Write<u16>(0);

      std::size_t offset_pos = Tell();
      Write<u32>(0);

      if (with_size)
        Write<u32>(0);

      return offset_pos;
    }

    Write<u32>(0);
    Write<s32>(-1);
    if (with_size)
      Write<u32>(0);

    return 0;
  }

  template <typename ValueType = s32>
  std::size_t WritePendingValue() {
    std::size_t value_offset = Tell();
    Write<ValueType>(0);
    return value_offset;
  }

  void WriteSamples(const audio::Channel& channel, audio::SampleFormat format) {
    switch (format) {
    case audio::SampleFormat::PCMS8:
      for (auto& sample : channel)
        Write(std::get<s8>(sample));
      break;
    case audio::SampleFormat::PCMS16:
      for (auto& sample : channel)
        Write(std::get<s16>(sample));
      break;
    case audio::SampleFormat::DSPADPCM:
      for (auto& sample : channel)
        Write(std::get<u8>(sample));
      break;
    case audio::SampleFormat::PCMS32:
      for (auto& sample : channel)
        Write(std::get<s32>(sample));
      break;
    }
  }
};
}  // namespace oead::util
