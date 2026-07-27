#pragma once

#include "oead/audio/audio_reader.h"
#include "oead/audio/audio_writer.h"

namespace oead::audio {
class IAssetFile {
public:
  virtual ~IAssetFile() = default;

  virtual void Deserialize(util::AudioReader& reader) = 0;
  virtual void Serialize(util::AudioWriter& writer) const = 0;
  virtual std::vector<u8> ToBinary() const = 0;
  virtual std::vector<u8> ToBinary(util::Endianness) const = 0;

  virtual util::Endianness Endianness() const = 0;
  virtual void Endianness(util::Endianness endian) = 0;
};
} // namespace oead::audio