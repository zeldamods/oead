#pragma once

#include "oead/audio/fstm.h"
#include "oead/util/swap.h"

namespace oead::audio::fstp {
struct PrefetchData {
  uint32_t start_frame;
  uint32_t prefetch_size;
  uint32_t reserved;

  // offset is relative to the start of the PrefetchData
  Reference to_prefetch_samples;

  OEAD_DEFINE_FIELDS(PrefetchData, start_frame, prefetch_size, reserved,
                     to_prefetch_samples);
};

struct PrefetchDataBlock {
  std::vector<PrefetchData> prefetch_metadata;
  std::vector<std::vector<std::uint8_t>> sample_data;

  PrefetchDataBlock() = default;
};

class Fstp : public IAssetFile {
public:
  Fstp() = default;
  Fstp(tcb::span<const u8> data);
  
  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;

  std::vector<u8> ToBinary() const override;

  const auto& Info() const { return m_info; }
  const auto& Data() const { return m_data; }

  void Endianness(util::Endianness endian) override { m_endian = endian; }
  util::Endianness Endianness() const override { return m_endian; }

  static int GetAlignment(util::Endianness endian) { return endian == util::Endianness::Little ? 0x40 : 0x20; }

private:
  void SerializeDataBlock(util::AudioWriter& writer) const;

  u32 m_version;
  fstm::InfoBlock m_info;
  PrefetchDataBlock m_data;
  
  util::Endianness m_endian;
};
} // namespace oead::audio::fstp