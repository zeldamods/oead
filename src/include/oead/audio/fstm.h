#pragma once

#include "oead/audio/dspadpcm.h"
#include "oead/audio/element_type.h"
#include "oead/audio/interface.h"

namespace oead::audio::fstm {
struct TrackInfo {
  std::uint8_t volume {0};
  std::uint8_t pan {0};
  std::uint8_t span {0};
  std::uint8_t flags {0};

  OEAD_DEFINE_FIELDS(TrackInfo, volume, pan, span, flags);
};

struct StreamSoundInfo {
  SampleFormat encoding{SampleFormat::DSPADPCM};
  bool is_loop{0};
  uint8_t channel_count{0};
  uint8_t region_count{0};
  uint32_t sample_rate{48000};
  uint32_t loop_start{0};
  uint32_t frame_count{0};
  uint32_t block_count{0};
  uint32_t block_size{0};
  uint32_t block_sample_count{0};
  uint32_t last_block_size{0};
  uint32_t last_block_sample_count{0};
  uint32_t last_block_padding_size{0};
  uint32_t size_of_seek_info_atom{0};
  uint32_t seek_info_interval_samples{0};
  Reference to_sample_data{(uint16_t)ElementType::General_ByteStream, 0}; // relative to the start of the data section
  uint16_t region_info_size{0};
  // padding[2];
  Reference to_region_block{(uint16_t)ElementType::StreamSoundFile_RegionBlock, 0};
  uint32_t original_loop_start;
  uint32_t original_loop_end;

  OEAD_DEFINE_FIELDS(StreamSoundInfo, encoding, is_loop, channel_count,
                     region_count, sample_rate, loop_start, frame_count,
                     block_count, block_size, block_sample_count,
                     last_block_size, last_block_sample_count,
                     last_block_padding_size, size_of_seek_info_atom, seek_info_interval_samples,
                     to_sample_data, region_info_size, to_region_block, 
                     original_loop_start, original_loop_end);
};

struct SeekInfo {
  std::int16_t yn1;
  std::int16_t yn2;

  OEAD_DEFINE_FIELDS(SeekInfo, yn1, yn2);
};

struct RegionInfo {
  uint32_t start;
  uint32_t end;
  std::array<DspAdpcmLoopParam, 16> adpcm_context;
  bool is_enabled;
  uint8_t padding[87];
  std::array<char, 64> region_name;

  OEAD_DEFINE_FIELDS(RegionInfo, start, end, adpcm_context, 
                     is_enabled, padding, region_name);
};

class InfoBlock {
public:
  InfoBlock() = default;
  InfoBlock(util::AudioReader& reader);

  void Serialize(util::AudioWriter& writer) const;

  StreamSoundInfo StreamInfo() const { return m_stream_info; }
  const auto& TrackInfos() const { return m_track_infos; }
  const auto& DetailChannelInfos() const { return m_detail_channel_infos; }

private:
  StreamSoundInfo m_stream_info;
  std::vector<TrackInfo> m_track_infos;
  std::vector<DspAdpcmInfo> m_detail_channel_infos;
};

class Fstm : public IAssetFile {
public:
  Fstm() = default;
  Fstm(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;
  
  std::vector<u8> ToBinary() const override;

  const auto& Info() const { return m_info; }
  const auto& SeekInfos() const { return m_seek_infos; }
  const auto& RegionInfos() const { return m_region_infos; }
  const auto& Samples() const { return m_samples; }

  bool HasRegion() const { return m_has_region; }

  void Endianness(util::Endianness endian) override { m_endian = endian; }
  util::Endianness Endianness() const override { return m_endian; }

private:
  void SerializeSeekBlock(util::AudioWriter& writer) const;
  void SerializeRegionBlock(util::AudioWriter& writer) const;
  void SerializeDataBlock(util::AudioWriter& writer) const;

  u32 m_version;
  InfoBlock m_info;
  bool m_has_region {false};
  util::Endianness m_endian;

  std::vector<std::vector<SeekInfo>> m_seek_infos;
  std::vector<RegionInfo> m_region_infos;
  std::vector<Channel> m_samples;
};
} // namespace oead::audio::fstm