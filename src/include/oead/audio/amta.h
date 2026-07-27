#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "oead/errors.h"
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
  std::uint32_t asset_name_offset{0};
  std::uint32_t sample_count{0};
  AssetType type {AssetType::Wave};
  std::uint8_t channel_count{0};
  std::uint8_t used_stream_tracks{0}; // Up to 8
  std::uint8_t flags{0};
  float unknown{0};
  std::uint32_t sample_rate{48000};
  std::uint32_t loop_start_frame{0};
  std::uint32_t loop_end_frame{0};
  float volume{1};

  struct StreamTrack {
    uint32_t channel_count{0};
    float volume{1};
    OEAD_DEFINE_FIELDS(StreamTrack, channel_count, volume);
  };
  std::array<StreamTrack, 8> stream_tracks;

  // Only in Version 4.0
  float amplitude_peak{1};
};

// Audio Meta
// File containing metadata for an audio asset 
// (either Wave or Stream)
class Amta {
public:
  struct StreamTrack {
    std::uint32_t channel_count{0};
    float volume{1};
    OEAD_DEFINE_FIELDS(StreamTrack, channel_count, volume);
  };

  Amta() = default;
  Amta(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader);
  void Serialize(util::AudioWriter& writer) const;

  /// Serialize to a .bameta file of the original endianness
  std::vector<u8> ToBinary() const;
  /// Serialize to a .bameta file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const;

  /// Get the asset name
  auto AssetName() const { return m_asset_name; }
  /// Set the asset name
  void AssetName(std::string asset_name) { m_asset_name = asset_name; }

  /// Get the file version the object will serialize to
  auto Version() const { return m_version; }
  /// Set the file version the object will serialize to
  void Version(u16 version) { m_version = version; }

  /// Get the number of samples within the asset
  auto SampleCount() const { return m_sample_count; }
  /// Set the number of samples within the asset
  void SampleCount(u32 sample_count) { m_sample_count = sample_count; }

  /// Get the type of the asset
  auto Type() const { return m_asset_type; }
  /// Set the type of the asset
  void Type(AssetType type) { m_asset_type = type; }

  /// Get the amount of channels used by the asset
  auto ChannelCount() const { return m_channel_count; }
  /// Set the amount of channels used by the asset
  void ChannelCount(u8 channel_count) { m_channel_count = channel_count; }

  auto UsedStreamTracks() const { return m_used_stream_tracks; }
  void UsedStreamTracks(u8 used_tracks) { m_used_stream_tracks = used_tracks; }

  // TODO: find out what the flags mean and modify them
  // individually

  /// Get the flag used by the asset
  auto Flags() const { return m_flags; }
  /// Set the flag used by the asset
  void Flags(u8 flag_byte) { m_flags = flag_byte; }

  /// Get an unknown value of the asset
  /// This value can be found in the DATA
  /// block of the meta file, at offset 0x14
  auto Unknown() const { return m_unknown; }
  /// Set an unknown value of the asset
  void Unknown(float duration) { m_unknown = duration; }

  /// Get the sample rate the asset will play at
  auto SampleRate() const { return m_sample_rate; }
  /// Set the sample rate the asset will play at
  void SampleRate(u32 sample_rate) { m_sample_rate = sample_rate; }

  /// Get the frame where the loop will start, if the asset is looped
  auto LoopStartFrame() const { return m_loop_start_frame; }
  /// Set the frame where the loop will start, if the asset is looped
  void LoopStartFrame(u32 loop_start_frame) {
    if (loop_start_frame > m_loop_end_frame)
      throw InvalidDataError("AudioMeta: Loop start frame is higher than end frame");

    m_loop_start_frame = loop_start_frame;
  }
  
  /// Get the frame where the loop will end, if the asset is looped
  auto LoopEndFrame() const { return m_loop_end_frame; }
  /// Set the frame where the loop will end, if the asset is looped
  void LoopEndFrame(u32 loop_end_frame) {
    if (loop_end_frame < m_loop_start_frame)
      throw InvalidDataError("AudioMeta: Loop end frame is lower than start frame");

    m_loop_start_frame = loop_end_frame;
  }

  /// Get the volume in decibels
  auto Volume() const { return m_volume; }
  /// Set the volume in decibels
  void Volume(float volume) { m_volume = volume; }

  /// Get all StreamTracks
  const auto& StreamTracks() const { return m_stream_tracks; }
  /// Set all StreamTracks
  void StreamTracks(std::array<StreamTrack, 8> tracks) { m_stream_tracks = tracks; }

  /// Get a StreamTracks by index
  auto GetStreamTrack(int idx) { return m_stream_tracks[idx]; }
  /// Set a StreamTracks by index
  void SetStreamTrack(StreamTrack track, int idx) { m_stream_tracks[idx] = track; }

  /// Get the amplitude peak
  auto AmplitudePeak() const { return m_amplitude_peak; }
  /// Set the amplitude peak
  void AmplitudePeak(float amplitude_peak) { m_amplitude_peak = amplitude_peak; }

  /// Get all MarkerInfos
  const auto& Markers() const { return m_markers; }
  /// Set all MarkerInfos
  void Markers(const std::vector<MarkerInfo>& markers) { m_markers = markers; }

  /// Get a MarkerInfo by index
  auto GetMarker(int idx) const { return m_markers[idx]; }
  /// Set a MarkerInfo by index
  void SetMarker(MarkerInfo marker, int idx) { m_markers[idx] = marker; }

  /// Get all ExtEntries
  const auto& ExtEntries() const { return m_ext_entries; }
  /// Set all ExtEntries
  void ExtEntries(const std::vector<ExtEntry>& ext_entries) { m_ext_entries = ext_entries; }

  /// Get an ExtEntry by index
  auto GetExtEntry(int idx) const { return m_ext_entries[idx]; }
  /// Set an ExtEntry by index
  void SetExtEntry(ExtEntry entry, int idx) { m_ext_entries[idx] = entry; }

  /// Get the endianness the object will serialize to
  auto Endianness() const { return m_endian; }
  /// Set the endianness the object will serialize to
  void Endianness(util::Endianness endianness) { m_endian = endianness; }
  
private:
  void DeserializeMarkerBlock(util::AudioReader& reader, u32 to_string_table);
  void DeserializeExtBlock(util::AudioReader& reader, u32 to_string_table);

  void SerializeData(util::AudioWriter& writer) const;
  std::vector<std::size_t> SerializeMarker(util::AudioWriter& writer) const;
  std::vector<std::size_t> SerializeExt(util::AudioWriter& writer) const;
  void SerializeStringTable(util::AudioWriter& writer, 
                            std::vector<std::size_t> marker_offsets, 
                            std::vector<std::size_t> ext_offsets) const;

  std::string m_asset_name {""};
  std::uint16_t m_version {0x400};
  std::uint32_t m_sample_count {0};
  AssetType m_asset_type {AssetType::Wave};
  std::uint8_t m_channel_count {1};
  std::uint8_t m_used_stream_tracks {0};
  std::uint8_t m_flags {0};
  float m_unknown {0};
  std::uint32_t m_sample_rate {48000};
  std::uint32_t m_loop_start_frame {0};
  std::uint32_t m_loop_end_frame {0};
  float m_volume {0};
  std::array<StreamTrack, 8> m_stream_tracks;
  float m_amplitude_peak {0};
  std::vector<MarkerInfo> m_markers;
  std::vector<ExtEntry> m_ext_entries;
  util::Endianness m_endian {util::Endianness::Little};
};
} // namespace oead::audio