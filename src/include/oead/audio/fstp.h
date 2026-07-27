#pragma once

#include <sstream>

#include "oead/audio/fstm.h"
#include "oead/errors.h"

namespace oead::audio::fstp {
struct PrefetchData {
  std::uint32_t start_frame {0};
  std::uint32_t prefetch_size {0};
  std::uint32_t reserved {0};

  // offset is relative to the start of the PrefetchData
  Reference to_prefetch_samples {};

  OEAD_DEFINE_FIELDS(PrefetchData, start_frame, prefetch_size, reserved,
                     to_prefetch_samples);
};

struct PrefetchDataBlock {
  PrefetchDataBlock() = default;

  std::vector<PrefetchData> prefetch_metadata;
  std::vector<std::vector<std::uint8_t>> sample_data;
};

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
  void Data(const PrefetchDataBlock& data) {
    if (data.sample_data.size() > data.prefetch_metadata.size())
      throw InvalidDataError("There not enough metadata entries compared to groups of sample data");

    if (data.sample_data.size() < data.prefetch_metadata.size())
      throw InvalidDataError("There too many metadata entries compared to groups of sample data");

    for (u32 i {0}; i < data.sample_data.size(); ++i) {
      if (data.sample_data[i].size() != data.prefetch_metadata[i].prefetch_size) {
        std::ostringstream oss;
        oss << "Sample data at index " << i 
        << "(size=" << data.sample_data[i].size() 
        << ") must have the size described by its corresponding metadata (idx=" << i 
        << ", size=" << data.prefetch_metadata[i].prefetch_size << ")";
        throw InvalidDataError(oss.str());
      }
    }

    m_data = data; 
  }

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