#pragma once

#include <memory>
#include <nonstd/span.h>

#include "oead/audio/amta.h"

#include "oead/audio/interface.h"
#include "oead/util/hash.h"

namespace oead::audio::bars {
struct FileOffsetSet {
  std::int32_t meta_offset;
  std::int32_t asset_offset;
  OEAD_DEFINE_FIELDS(FileOffsetSet, meta_offset, asset_offset);
};

struct ResourceHeader {
  std::array<char, 4> signature {'B', 'A', 'R', 'S'};
  std::uint32_t file_size{};
  std::uint16_t bom{0xFEFF};
  std::uint16_t version{};
  std::uint32_t asset_count{};

  OEAD_DEFINE_FIELDS(ResourceHeader, signature, file_size, bom, version, asset_count);
};

/// Binary Audio Resource
/// Contains embedded pairs of Amta (AudioMeta) and either
/// Fstp (Stream Prefetch) or Fwav (Wave) audio files
class Bars {
public:
  struct FileWithMetadata {
    amta::Amta metadata;
    std::shared_ptr<IAssetFile> asset;
  };

  Bars() = default;
  Bars(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader);
  void Serialize(util::AudioWriter& writer) const;

  /// Serialize to a .bars file of the original endianness
  std::vector<u8> ToBinary() const;
  /// Serialize to a .bars file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const;

  auto Version() const { return m_version; }

  const auto& GetFiles() const { return m_files; }
  
  const auto& GetFile(int idx) const { return m_files[idx]; }
  const auto& GetFile(std::string name) const {
    std::uint32_t hash {util::crc32(name)};
    auto iter {std::lower_bound(m_hashes.begin(), m_hashes.end(), hash)};
    if (iter == m_hashes.end())
      throw std::runtime_error("Name not found");
    
    return GetFile(std::distance(m_hashes.begin(), iter));
  }

  void SwapEndianness();

private:
  u16 m_version {};
  std::vector<u32> m_hashes;
  std::vector<FileOffsetSet> m_offset_sets;
  std::vector<FileWithMetadata> m_files;
  util::Endianness m_endianness {util::Endianness::Little};
};
} // namespace oead::bars