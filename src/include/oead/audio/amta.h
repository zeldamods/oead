#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "oead/types.h"
#include "oead/audio/audio_reader.h"
#include "oead/audio/audio_writer.h"
#include "oead/util/swap.h"

namespace oead::audio::amta {
struct MarkerInfo {
  std::uint32_t id;
  std::string name;
  std::uint32_t start_pos;
  std::uint32_t length;
};

struct MarkerInfoBin {
  std::uint32_t id;
  std::uint32_t asset_name_offset;
  std::uint32_t start_pos;
  std::uint32_t length;
};

struct ExtEntry {
  std::string name;
  std::uint32_t value;
};

struct ExtEntryBin {
  std::uint32_t name_offset;
  std::uint32_t value;
};

struct AudioMetaHeader {
  std::array<char, 4> signature{'A', 'M', 'T', 'A'};
  std::uint16_t bom;
  std::uint16_t version;
  std::uint32_t file_size;
  std::uint32_t data_offset;
  std::uint32_t mark_offset;
  std::uint32_t ext_offset;
  std::uint32_t strg_offset;

  OEAD_DEFINE_FIELDS(AudioMetaHeader, signature, bom, version, file_size, 
                     data_offset, mark_offset, ext_offset, strg_offset);
};

struct AudioMetaDataBin {
  BlockHeader header{{'D', 'A', 'T', 'A'}, 0x64};
  uint32_t asset_name_offset{0};
  uint32_t sample_count{0};
  AssetType type {AssetType::Wave};
  uint8_t channel_count{0};
  uint8_t used_stream_tracks{0}; // Up to 8
  uint8_t flags{0};
  uint32_t duration{0};
  uint32_t sample_rate{48000};
  uint32_t loop_start_frame{0};
  uint32_t loop_end_frame{0};
  float loudness{1};

  struct StreamTrack {
    uint32_t channel_count{0};
    float volume{1};
    OEAD_DEFINE_FIELDS(StreamTrack, channel_count, volume);
  };
  std::array<StreamTrack, 8> stream_tracks;

  // Only in Version 4.0
  float amplitude_peak{1};
};

class Amta {
public:
  struct StreamTrack {
    uint32_t channel_count{0};
    float volume{1};
    OEAD_DEFINE_FIELDS(StreamTrack, channel_count, volume);
  };

  Amta() = default;
  Amta(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader);
  void Serialize(util::AudioWriter& writer) const;

  std::vector<u8> ToBinary() const;

  auto AssetName() const { return m_asset_name; }
  auto Version() const { return m_version; }
  auto Type() const { return m_asset_type; }

  const auto& Markers() const { return m_markers; }
  const auto& ExtValues() const { return m_ext_entries; }

  void Endianness(util::Endianness endianness) { m_endianness = endianness; }
  auto Endianness() const { return m_endianness; }
  
private:
  void DeserializeMarkerBlock(util::AudioReader& reader, u32 to_string_table);
  void DeserializeExtBlock(util::AudioReader& reader, u32 to_string_table);

  void SerializeData(util::AudioWriter& writer) const;
  std::vector<std::size_t> SerializeMarker(util::AudioWriter& writer) const;
  std::vector<std::size_t> SerializeExt(util::AudioWriter& writer) const;
  void SerializeStringTable(util::AudioWriter& writer, 
                            std::vector<std::size_t> marker_offsets, 
                            std::vector<std::size_t> ext_offsets) const;

  std::string m_asset_name;
  std::uint16_t m_version;
  std::uint32_t m_sample_count;
  AssetType m_asset_type;
  std::uint8_t m_channel_count;
  std::uint8_t m_used_stream_tracks;
  std::uint8_t m_flags;
  std::uint32_t m_duration;
  std::uint32_t m_sample_rate;
  std::uint32_t m_loop_start_frame;
  std::uint32_t m_loop_end_frame;
  float m_loudness;
  std::array<StreamTrack, 8> m_stream_tracks;
  float m_amplitude_peak;
  std::vector<MarkerInfo> m_markers;
  std::vector<ExtEntry> m_ext_entries;
  util::Endianness m_endianness;
};
} // namespace oead::audio