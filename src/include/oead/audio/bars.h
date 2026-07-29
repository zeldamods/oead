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
    amta::Amta meta;
    std::shared_ptr<IAssetFile> asset;
  };

  Bars() = default;
  Bars(tcb::span<const u8> data);
  Bars(const std::string& file_path);

  void Deserialize(util::AudioReader& reader);
  void Serialize(util::AudioWriter& writer) const;

  /// Serialize to a .bars file of the original endianness
  std::vector<u8> ToBinary() const;
  /// Serialize to a .bars file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const;

  /// Get a file's meta by index and serialize only that file's meta
  std::vector<u8> MetaToBinary(int idx) const;
  /// Get a file's meta by name and serialize only that file's meta
  std::vector<u8> MetaToBinary(const std::string& name) const;
  
  /// Get a file by index and serialize only that file's data
  std::vector<u8> FileToBinary(int idx) const;
  /// Get a file by name and serialize only that file's data
  std::vector<u8> FileToBinary(const std::string& name) const;

  /// Get an array of all files
  const auto& GetFiles() const { return m_files; }

  /// Get a file by index
  const auto& GetFile(int idx) const { return m_files[idx]; }
  /// Get a file by name
  const auto& GetFile(const std::string& name) const {
    std::uint32_t hash {util::crc32(name)};
    auto iter {std::lower_bound(m_hashes.begin(), m_hashes.end(), hash)};
    if (iter == m_hashes.end())
      throw InvalidDataError("Name not found");
    
    return GetFile(std::distance(m_hashes.begin(), iter));
  }

  /// Add a file alongside its metadata
  void AddFile(const amta::Amta& meta, const std::shared_ptr<IAssetFile> file) {
    FileWithMetadata new_file {};
    new_file.meta = meta;
    if (file != nullptr)
      new_file.asset = file;

    u32 name_hash {util::crc32(new_file.meta.AssetName())};

    m_hashes.push_back(name_hash);
    std::sort(m_hashes.begin(), m_hashes.end());

    auto iter {std::lower_bound(m_hashes.begin(), m_hashes.end(), name_hash)};

    m_files.insert(m_files.begin() + std::distance(m_hashes.begin(), iter), new_file);
  }

  /// Get the file version the object will serialize to
  auto Version() const { return m_version; }
  /// Set the file version the object will serialize to
  void Version(u16 version) { m_version = version; }

  /// Get the endianness the object will serialize to
  auto Endianness() const { return m_endian; }
  /// Set the endianness of the object, and the 
  /// endianness of all sub files
  void Endianness(util::Endianness endianness) { 
    m_endian = endianness;

    for (auto& file : m_files) {
      file.meta.Endianness(m_endian);
      if (file.asset != nullptr) {
        file.asset->Endianness(m_endian);
      }
    }
  }

  /// Swap the endianness
  void SwapEndianness();

private:
  std::uint16_t m_version {};
  std::vector<std::uint32_t> m_hashes;
  std::vector<FileOffsetSet> m_offset_sets;
  std::vector<FileWithMetadata> m_files;
  util::Endianness m_endian {util::Endianness::Little};
};
} // namespace oead::bars