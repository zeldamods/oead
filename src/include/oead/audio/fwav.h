#pragma once

#include "oead/audio/dspadpcm.h"
#include "oead/audio/interface.h"
#include "oead/errors.h"

namespace oead::audio::fwav {
struct ChannelInfo {
  DspAdpcmInfo adpcm_info {};

  OEAD_DEFINE_FIELDS(ChannelInfo, adpcm_info);
};

struct InfoBlock {
  SampleFormat encoding {SampleFormat::DSPADPCM};
  bool is_loop {0};
  std::uint32_t sample_rate {48000};
  std::uint32_t loop_start_frame {0};
  std::uint32_t loop_end_frame {0};
  std::uint32_t original_loop_start_frame {0};
  
  OEAD_DEFINE_FIELDS(InfoBlock, encoding, is_loop, sample_rate, loop_start_frame, 
                     loop_end_frame, original_loop_start_frame);
};

/// Wave
class Fwav : public IAssetFile {
public:
  Fwav() = default;
  Fwav(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;

  /// Serialize to a .bfwav file of the original endianness
  std::vector<u8> ToBinary() const override;
  /// Serialize to a .bfwav file of a specific endianness
  std::vector<u8> ToBinary(util::Endianness endian) const override;
  
  /// Get the format of the samples
  auto Encoding() const { return m_encoding; }
  /// Set the format of the samples
  void Encoding(SampleFormat format) { m_encoding = format; }

  /// Check if the audio is looping
  bool isLoop() const { return m_is_loop; }
  /// Set the audio's looping state
  void isLoop(bool is_loop) { m_is_loop = is_loop; }

  /// Get the sample rate the asset will play at
  auto SampleRate() const { return m_sample_rate; }
  /// Set the sample rate the asset will play at
  void SampleRate(u32 sample_rate) { m_sample_rate = sample_rate; }

  /// Get the frame where the loop will start, if the asset is looped
  auto LoopStartFrame() const { return m_loop_start_frame; }
  /// Set the frame where the loop will start, if the asset is looped
  void LoopStartFrame(u32 loop_start_frame) {
    if (loop_start_frame > m_loop_end_frame)
      throw InvalidDataError("Wave: Loop start frame is higher than end frame");

    m_loop_start_frame = loop_start_frame;
  }
  
  /// Get the frame where the loop will end, if the asset is looped
  auto LoopEndFrame() const { return m_loop_end_frame; }
  /// Set the frame where the loop will end, if the asset is looped
  void LoopEndFrame(u32 loop_end_frame) {
    if (loop_end_frame < m_loop_start_frame)
      throw InvalidDataError("Wave: Loop end frame is lower than start frame");

    m_loop_start_frame = loop_end_frame;
  }

  /// Get the original loop start frame
  auto OriginalLoopStartFrame() const { return m_original_loop_start_frame; }
  /// Set the original loop start frame
  void OriginalLoopStartFrame(u32 original_loop_start_frame) { m_original_loop_start_frame = original_loop_start_frame; }

  /// Get all channel infos
  const auto& ChannelInfos() const { return m_channel_infos; }
  /// Set all channel infos
  void ChannelInfos(const std::vector<ChannelInfo>& channel_infos) { m_channel_infos = channel_infos; }
  
  /// Get a channel info by the index of the channel
  auto GetChannelInfo(int channel) const { return m_channel_infos[channel]; }
  /// Set a channel info by the index of the channel
  void SetChannelInfo(ChannelInfo info, int channel) { m_channel_infos[channel] = info; }

  /// Get all samples
  const auto& Samples() const { return m_samples; }
  /// Set all samples
  void Samples(const std::vector<Channel>& samples) { m_samples = samples; }

  /// Get the endianness the object will serialize to
  util::Endianness Endianness() const override { return m_endian; }
  /// Set the endianness the object will serialize to
  void Endianness(util::Endianness endian) override { m_endian = endian; }

  /// Get the alignment of a WaveFile based on endianness
  static int GetAlignment(util::Endianness endian) { return endian == util::Endianness::Little ? 0x40 : 0x20; }

private:
  void DeserializeInfoBlock(util::AudioReader& reader);
  void DeserializeDataBlock(util::AudioReader& reader);

  std::vector<std::size_t> SerializeInfoBlock(util::AudioWriter& writer) const;
  void SerializeDataBlock(util::AudioWriter& writer, 
                          const std::vector<std::size_t>& samples_offset_pos) const;

  u32 m_version {0x10200};
  SampleFormat m_encoding {SampleFormat::DSPADPCM};
  bool m_is_loop {false};
  u32 m_sample_rate {48000};
  u32 m_loop_start_frame {0};
  u32 m_loop_end_frame {0};
  u32 m_original_loop_start_frame {0};
  std::vector<ChannelInfo> m_channel_infos;
  std::vector<Channel> m_samples;
  util::Endianness m_endian {util::Endianness::Little};
};
} // namespace nn::audio::fwav