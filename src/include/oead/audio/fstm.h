#pragma once

#include "oead/audio/dspadpcm.h"
#include "oead/audio/interface.h"

namespace oead::audio::fstm {
struct StreamSoundInfo {
  SampleFormat encoding{SampleFormat::DSPADPCM};
  bool is_loop {0};
  std::uint8_t channel_count {0};
  std::uint8_t region_count {0};
  std::uint32_t sample_rate {48000};
  std::uint32_t loop_start {0};
  std::uint32_t frame_count {0};
  std::uint32_t block_count {0};
  std::uint32_t block_size {0};
  std::uint32_t block_sample_count {0};
  std::uint32_t last_block_size {0};
  std::uint32_t last_block_sample_count {0};
  std::uint32_t last_block_padding_size {0};
  std::uint32_t size_of_seek_info_atom {0};
  std::uint32_t seek_info_interval_samples {0};
  Reference to_sample_data {ElementType::General_ByteStream, 0}; // relative to the start of the data section
  std::uint16_t region_info_size {0};
  // padding[2];
  Reference to_region_block {ElementType::StreamSoundFile_RegionBlock, 0};
  std::uint32_t original_loop_start {0};
  std::uint32_t original_loop_end {0};

  OEAD_DEFINE_FIELDS(StreamSoundInfo, encoding, is_loop, channel_count,
                     region_count, sample_rate, loop_start, frame_count,
                     block_count, block_size, block_sample_count,
                     last_block_size, last_block_sample_count,
                     last_block_padding_size, size_of_seek_info_atom, seek_info_interval_samples,
                     to_sample_data, region_info_size, to_region_block, 
                     original_loop_start, original_loop_end);
};

struct SeekInfo {
  std::int16_t yn1 {0};
  std::int16_t yn2 {0};

  OEAD_DEFINE_FIELDS(SeekInfo, yn1, yn2);
};

struct RegionInfo {
  std::uint32_t start {0};
  std::uint32_t end {0};
  std::array<DspAdpcmLoopParam, 16> adpcm_context;
  bool is_enabled {false};
  std::uint8_t padding[87] {};
  std::array<char, 64> region_name {};

  OEAD_DEFINE_FIELDS(RegionInfo, start, end, adpcm_context, 
                     is_enabled, padding, region_name);
};

struct TrackInfo {
  std::uint8_t volume {0};
  std::uint8_t pan {0};
  std::uint8_t span {0};
  std::uint8_t flags {0};

  OEAD_DEFINE_FIELDS(TrackInfo, volume, pan, span, flags);
};

class InfoBlock {
public:
  InfoBlock() = default;
  InfoBlock(util::AudioReader& reader);

  void Serialize(util::AudioWriter& writer) const;

  auto StreamInfo() const { return m_stream_info; }
  void StreamInfo(StreamSoundInfo stream_info) { m_stream_info = stream_info; }

  const auto& TrackInfos() const { return m_track_infos; }
  void TrackInfos(const std::vector<TrackInfo>& track_infos) { m_track_infos = track_infos; }

  auto GetTrackInfo(int track_idx) const { return m_track_infos[track_idx]; }
  void SetTrackInfo(TrackInfo info, int track_idx) { m_track_infos[track_idx] = info; }

  const auto& DetailChannelInfos() const { return m_detail_channel_infos; }
  void DetailChannelInfos(const std::vector<DspAdpcmInfo>& channel_infos) { m_detail_channel_infos = channel_infos; }

  auto GetDetailChannelInfo(int channel) const { return m_detail_channel_infos[channel]; }
  void SetDetailChannelInfo(DspAdpcmInfo info, int channel) { m_detail_channel_infos[channel] = info; }

private:
  StreamSoundInfo m_stream_info;
  std::vector<TrackInfo> m_track_infos;
  std::vector<DspAdpcmInfo> m_detail_channel_infos;
};

/// Stream Sound
class Fstm : public IAssetFile {
public:
  Fstm() = default;
  Fstm(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;
  
  /// Serialize to a .bfstm file of the original endianness
  std::vector<u8> ToBinary() const override;
  /// Serialize to a .bfstm file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const override;

  /// Get the file version the object will serialize to
  auto Version() const { return m_version; }
  /// Set the file version the object will serialize to
  void Version(u32 version) { m_version = version; }

  /// Get the InfoBlock
  const auto& Info() const { return m_info; }
  /// Set the InfoBlock
  void Info(InfoBlock info) { m_info = info; }

  /// Get all SeekInfos
  const auto& SeekInfos() const { return m_seek_infos; }
  /// Set all SeekInfos
  void SeekInfos(const std::vector<std::vector<SeekInfo>>& seek_infos) { m_seek_infos = seek_infos; }

  /// Get a single SeekInfos
  auto GetSeekInfo(int block, int channel) const { return m_seek_infos[block][channel]; }
  /// Set a single SeekInfos
  void SetSeekInfo(SeekInfo seek_info, int block, int channel) { m_seek_infos[block][channel] = seek_info; }

  /// Get all RegionInfos
  const auto& RegionInfos() const { return m_region_infos; }
  /// Set all RegionInfos
  void RegionInfos(const std::vector<RegionInfo>& region_infos) {
    if (!region_infos.empty())
      m_has_region = true;
    else
      m_has_region = false;
    m_region_infos = region_infos; 
  }

  /// Get a single RegionInfo by index
  auto GetRegionInfo(int idx) const { return m_region_infos[idx]; }
  /// Set a single RegionInfo by index
  void SetRegionInfo(RegionInfo info, int idx) { m_region_infos[idx] = info; }

  /// Get all samples
  const auto& Samples() const { return m_samples; }
  /// Set all samples
  void Samples(const std::vector<Channel>& samples) { m_samples = samples; }

  /// Check if the file contains a region section
  bool HasRegion() const { return m_has_region; }

  /// Get the endianness the object will serialize to
  util::Endianness Endianness() const override { return m_endian; }
  /// Set the endianness the object will serialize to
  void Endianness(util::Endianness endian) override { m_endian = endian; }

private:
  void SerializeSeekBlock(util::AudioWriter& writer) const;
  void SerializeRegionBlock(util::AudioWriter& writer) const;
  void SerializeDataBlock(util::AudioWriter& writer) const;

  u32 m_version;
  InfoBlock m_info;
  bool m_has_region {false};
  
  // Unsure if channels per block, or blocks per channel
  std::vector<std::vector<SeekInfo>> m_seek_infos;

  std::vector<RegionInfo> m_region_infos;
  std::vector<Channel> m_samples;
  util::Endianness m_endian;
};
} // namespace oead::audio::fstm