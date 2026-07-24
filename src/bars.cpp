#include <oead/audio/bars.h>

#include "oead/errors.h"
#include "oead/util/magic_utils.h"

#include "oead/audio/fstp.h"
#include "oead/audio/fwav.h"
#include "oead/util/swap.h"

namespace oead::audio::bars {
constexpr auto BarsMagic = util::MakeMagic("BARS");

Bars::Bars(tcb::span<const u8> data) {
  util::AudioReader reader {data, util::Endianness::Little};
  Deserialize(reader);
}

void Bars::Deserialize(util::AudioReader& reader) {
  std::size_t file_start {reader.Tell()};
  auto header = reader.Read<ResourceHeader>();

  if (util::ByteOrderMarkToEndianness(header.bom) == util::Endianness::Little) {
    reader.SwapEndianness();
    reader.Seek(file_start);
    header = reader.Read<ResourceHeader>();
  }

  if (header.signature != BarsMagic)
    throw InvalidDataError("Invalid BARS magic");
  
  if (header.version > 0x101)
    throw InvalidDataError("Unsupported BARS version"); 

  m_version = header.version;
  m_endianness = reader.Endian();

  m_hashes.resize(header.asset_count);
  for (auto& hash : m_hashes)
    hash = reader.Read<std::uint32_t>();

  m_offset_sets.resize(header.asset_count);
  for (auto& offset_set : m_offset_sets)
    offset_set = reader.Read<FileOffsetSet>();

  m_files.resize(header.asset_count);
  // TODO: describe what's happening next
  std::map<std::int32_t, std::shared_ptr<IAssetFile>> found_assets;
  for (uint i {0}; i < header.asset_count; ++i) {
    reader.Seek(m_offset_sets[i].meta_offset);
    m_files[i].metadata.Deserialize(reader);

    auto found_asset {found_assets.find(m_offset_sets[i].asset_offset)};

    if (found_asset != found_assets.end()) {
      m_files[i].asset = found_asset->second;
    }
    else if (m_offset_sets[i].asset_offset != -1) {
      reader.Seek(m_offset_sets[i].asset_offset);

      switch (m_files[i].metadata.Type()) {
      case oead::audio::AssetType::Wave: {
        auto fwav = std::make_shared<fwav::Fwav>();
        fwav->Deserialize(reader);
        m_files[i].asset = fwav;
        break;
      }
      case oead::audio::AssetType::Stream: {
        auto fstp = std::make_shared<fstp::Fstp>();
        fstp->Deserialize(reader);
        m_files[i].asset = fstp;
        break;
      }
      case oead::audio::AssetType::Unknown:
        throw InvalidDataError("Invalid AssetType");
      }
    }
    else {
      m_files[i].asset = nullptr;
    }

    found_assets[m_offset_sets[i].asset_offset] = m_files[i].asset;
  }
}

void Bars::SwapEndianness() {
  if (m_endianness == util::Endianness::Little)
    m_endianness = util::Endianness::Big;
  else
    m_endianness = util::Endianness::Little;

  for (auto& file : m_files) {
    file.metadata.Endianness(m_endianness);
    if (file.asset != nullptr) {
      file.asset->Endianness(m_endianness);
    }
  }
}

std::vector<u8> Bars::ToBinary() const {
  util::AudioWriter writer {m_endianness};
  Serialize(writer);
  return writer.Finalize();
}

std::vector<u8> Bars::ToBinary(util::Endianness endian) const {
  util::AudioWriter writer {endian};
  Serialize(writer);
  return writer.Finalize();
}

void Bars::Serialize(util::AudioWriter& writer) const {
  std::size_t file_start {writer.Tell()};

  writer.WriteString("BARS");

  std::size_t file_size_pos {writer.WritePendingValue()};

  writer.Write<std::uint16_t>(0xFEFF);
  writer.Write<std::uint16_t>(m_version);
  writer.Write<std::uint32_t>(m_files.size());

  for (auto& hash : m_hashes)
    writer.Write(hash);

  std::vector<FileOffsetSet> offset_sets_pos(m_files.size());
  for (uint i {0}; i < m_files.size(); ++i) {
    offset_sets_pos[i].meta_offset = writer.WritePendingValue();
    if (m_files[i].asset == nullptr)
      writer.Write<std::int32_t>(-1);
    else
      offset_sets_pos[i].asset_offset = writer.WritePendingValue();
  }

  for (uint i{0}; i < m_files.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::uint32_t>(offset_sets_pos[i].meta_offset, file_start);
    m_files[i].metadata.Serialize(writer);
  }

  // TODO: describe what happens next
  std::map<std::shared_ptr<IAssetFile>, std::size_t> done_assets;
  for (uint i{0}; i < m_files.size(); ++i) {
    if (m_files[i].asset != nullptr) {
      auto it {done_assets.find(m_files[i].asset)};
      if (it != done_assets.end()) {
        std::size_t return_offset {writer.Tell()};
        writer.Seek(offset_sets_pos[i].asset_offset);
        writer.Write<std::int32_t>(it->second - file_start);
        writer.Seek(return_offset);
      }
      else {
        writer.AlignUp(0x40);
        writer.WriteCurrentOffsetAt<std::int32_t>(offset_sets_pos[i].asset_offset, file_start);
        done_assets[m_files[i].asset] = writer.Tell();
        switch (m_files[i].metadata.Type()) {
        case AssetType::Wave: {
          auto fwav = std::static_pointer_cast<fwav::Fwav>(m_files[i].asset);
          fwav->Serialize(writer);
          break;
        }
        case AssetType::Stream: {
          auto fstp = std::static_pointer_cast<fstp::Fstp>(m_files[i].asset);
          fstp->Serialize(writer);
          break;
        }
        case AssetType::Unknown:
          break;
        default:
          throw std::runtime_error("Invalid file type!");
        }
      }
    }
  }

  writer.WriteCurrentOffsetAt<std::uint32_t>(file_size_pos, file_start);
}
} // namespace oead::bars