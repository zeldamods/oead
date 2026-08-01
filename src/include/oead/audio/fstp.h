#pragma once

#include "oead/audio/fstm.h"

namespace oead::audio::fstp {
struct PrefetchDataBin {
  std::uint32_t start_frame {0};
  std::uint32_t prefetch_size {0};
  std::uint32_t reserved {0};

  // offset is relative to the start of the PrefetchData
  Reference to_prefetch_samples {};

  OEAD_DEFINE_FIELDS(PrefetchDataBin, start_frame, prefetch_size, reserved,
                     to_prefetch_samples);
};

struct PrefetchData {
  std::uint32_t start_frame {0};
  std::vector<std::uint8_t> prefetch_samples;
};

struct PrefetchDataBlock {
  PrefetchDataBlock() = default;

  std::vector<PrefetchData> prefetch_data;
};

/// Stream Prefetch Sound
class Fstp : public IAssetFile {
public:
  Fstp() = default;
  Fstp(tcb::span<const u8> data);
  
  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;

  /// Serialize to a .bfstp file of the original endianness
  std::vector<u8> ToBinary() const override;
  /// Serialize to a .bfstp file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const override;

  /// Get the file version the object will serialize to
  auto Version() const { return m_version; }
  /// Set the file version the object will serialize to
  void Version(u32 version) { m_version = version; }

  /// Get the stream's InfoBlock
  const auto& Info() const { return m_info; }
  /// Set the stream's InfoBlock
  void Info(fstm::InfoBlock info) { m_info = info; }

  /// Get the PrefetchDataBlock
  const auto& Data() const { return m_data; }
  /// Set the PrefetchDataBlock
  void Data(const PrefetchDataBlock& data) { m_data = data; }

  /// Get the endianness the object will serialize to
  util::Endianness Endianness() const override { return m_endian; }
  /// Set the endianness the object will serialize to
  void Endianness(util::Endianness endian) override { m_endian = endian; }

  /// Get the alignment of a StreamPrefetchFile based on endianness
  static int GetAlignment(util::Endianness endian) { return endian == util::Endianness::Little ? 0x40 : 0x20; }

private:
  void SerializeDataBlock(util::AudioWriter& writer) const;

  std::uint32_t m_version;
  fstm::InfoBlock m_info;
  PrefetchDataBlock m_data;
  util::Endianness m_endian;
};
} // namespace oead::audio::fstp