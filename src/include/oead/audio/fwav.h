#pragma once

#include "oead/audio/dspadpcm.h"
#include "oead/audio/interface.h"
#include "oead/util/swap.h"

namespace oead::audio::fwav {
struct ChannelInfo {
  DspAdpcmInfo adpcm_info;

  OEAD_DEFINE_FIELDS(ChannelInfo, adpcm_info);
};

struct WaveInfo {
  SampleFormat encoding {SampleFormat::DSPADPCM};
  bool is_loop {0};
  std::uint32_t sample_rate {48000};
  std::uint32_t loop_start_frame {0};
  std::uint32_t loop_end_frame {0};
  std::uint32_t original_loop_start_frame {0};
  
  OEAD_DEFINE_FIELDS(WaveInfo, encoding, is_loop, sample_rate, loop_start_frame, 
                     loop_end_frame, original_loop_start_frame);
};

class Fwav : public IAssetFile {
public:
  Fwav() = default;
  Fwav(tcb::span<const u8> data);

  void Deserialize(util::AudioReader& reader) override;
  void Serialize(util::AudioWriter& writer) const override;

  std::vector<u8> ToBinary() const override;
  
  SampleFormat GetEncoding() const { return m_encoding; }
  void SetEncoding(SampleFormat encoding) { m_encoding = encoding; }

  const auto& ChannelInfos() const { return m_channel_infos; }
  const auto& Samples() const { return m_samples; }

  void Endianness(util::Endianness endian) override { m_endianness = endian; }
  util::Endianness Endianness() const override { return m_endianness; }

  static int GetAlignment(util::Endianness endian) { return endian == util::Endianness::Little ? 0x40 : 0x20; }

private:
  void DeserializeInfoBlock(util::AudioReader& reader);
  void DeserializeDataBlock(util::AudioReader& reader);

  std::vector<std::size_t> SerializeInfoBlock(util::AudioWriter& writer) const;
  void SerializeDataBlock(util::AudioWriter& writer, 
                          const std::vector<std::size_t>& samples_offset_pos) const;

  u32 m_version;
  SampleFormat m_encoding;
  bool m_is_loop;
  u32 m_sample_rate;
  u32 m_loop_start_frame;
  u32 m_loop_end_frame;
  u32 m_original_loop_start_frame;
  std::vector<ChannelInfo> m_channel_infos;
  std::vector<Channel> m_samples;

  util::Endianness m_endianness;
};
} // namespace nn::audio::fwav