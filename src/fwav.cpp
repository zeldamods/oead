#include <oead/audio/fwav.h>

#include "oead/util/magic_utils.h"

namespace oead::audio::fwav {
constexpr auto FwavMagic = util::MakeMagic("FWAV");
constexpr auto InfoMagic = util::MakeMagic("INFO");
constexpr auto DataMagic = util::MakeMagic("DATA");

Fwav::Fwav(tcb::span<const u8> data) {
  util::AudioReader reader {data, util::Endianness::Little};
  Deserialize(reader);
}

void Fwav::Deserialize(util::AudioReader& reader) {
  std::size_t file_start {reader.Tell()};

  auto header = reader.ReadSoundFileHeader();

  if (header.signature != FwavMagic)
    throw InvalidDataError("Invalid FWAV magic");
  
  m_version = header.version;
  // Version 1.2.0
  if (m_version != 0x10200)
    throw InvalidDataError("Unsupported FWAV version");

  m_endian = reader.Endian();

  for (auto& ref : header.block_refs) {
    reader.Seek(file_start + ref.offset);

    auto element_type = static_cast<ElementType>(ref.type_id);

    switch (element_type) {
    case oead::audio::ElementType::WaveFile_InfoBlock:
      DeserializeInfoBlock(reader);
      break;
    case oead::audio::ElementType::WaveFile_DataBlock:
      DeserializeDataBlock(reader);
      break;
    default:
      break;
    }
  }
}

void Fwav::DeserializeInfoBlock(util::AudioReader& reader) {
  reader.Read<BlockHeader>();
  auto info = reader.Read<InfoBlock>();

  m_encoding = info.encoding;
  m_is_loop = info.is_loop;
  m_sample_rate = info.sample_rate;
  m_loop_start_frame = info.loop_start_frame;
  m_loop_end_frame = info.loop_end_frame;
  m_original_loop_start_frame = info.original_loop_start_frame;

  size_t channel_info_table_start = reader.Tell();
  Table<Reference> channel_info_ref_table = reader.ReadTable<Reference>();
  m_channel_infos.resize(channel_info_ref_table.count);

  for (uint i {0}; i < channel_info_ref_table.count; ++i) {
    Reference channel_info_ref = channel_info_ref_table.items[i];
    reader.Seek(channel_info_table_start + channel_info_ref.offset);

    size_t channel_info_start = reader.Tell();
    reader.Read<Reference>();
    Reference to_adpcm_info {reader.Read<Reference>()};
    reader.Read<std::uint32_t>(); // reserved

    reader.Seek(channel_info_start + to_adpcm_info.offset);
    m_channel_infos[i].adpcm_info = reader.Read<DspAdpcmInfo>();
  }
}

void Fwav::DeserializeDataBlock(util::AudioReader& reader) {
  reader.Read<BlockHeader>();

  m_samples.resize(m_channel_infos.size());
  reader.Align(GetAlignment(m_endian));

  for (uint i {0}; i < m_channel_infos.size(); ++i) {
    m_samples[i] = reader.ReadSamples(m_loop_end_frame, 
                                      m_encoding, 
                                      true);
    reader.Align(GetAlignment(m_endian));
  }
}

std::vector<u8> Fwav::ToBinary() const {
  util::AudioWriter writer {m_endian};
  Serialize(writer);
  return writer.Finalize();
}

std::vector<u8> Fwav::ToBinary(util::Endianness endian) const {
  util::AudioWriter writer {endian};
  Serialize(writer);
  return writer.Finalize();
}

void Fwav::Serialize(util::AudioWriter& writer) const {
  std::size_t file_start {writer.Tell()};
  
  std::map<std::string, std::vector<std::size_t>> pending_header_values {writer.WriteSoundFileHeader(
    FwavMagic, 
    m_version, 
    {
      ElementType::WaveFile_InfoBlock,
      ElementType::WaveFile_DataBlock,
    }
  )};

  // INFO
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][0], 
                                            file_start);
  std::size_t info_section_start {writer.Tell()};
  std::vector<std::size_t> samples_offset_pos = SerializeInfoBlock(writer);
  writer.WriteCurrentOffsetAt<std::uint32_t>(pending_header_values["blocks_size"][0], 
                                             info_section_start);

  // DATA
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][1], 
                                            file_start);
  std::size_t data_section_start {writer.Tell()};
  SerializeDataBlock(writer, samples_offset_pos);
  writer.WriteCurrentOffsetAt<std::uint32_t>(pending_header_values["blocks_size"][1], 
                                             data_section_start);

  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["file_size"][0], 
                                            file_start);
}

std::vector<std::size_t> Fwav::SerializeInfoBlock(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(InfoMagic);
  std::size_t section_size_pos {writer.WritePendingValue()};

  writer.Write(m_encoding);
  writer.Write(m_is_loop);
  writer.AlignUp(4);
  writer.Write(m_sample_rate);
  writer.Write(m_loop_start_frame);
  writer.Write(m_loop_end_frame);
  writer.Write(m_original_loop_start_frame);

  std::size_t channel_info_ref_table_start {writer.Tell()};
  writer.Write<std::uint32_t>(m_channel_infos.size());

  std::vector<std::size_t> channel_infos_offset_pos(m_channel_infos.size());
  for (uint i {0}; i < m_channel_infos.size(); ++i) {
    channel_infos_offset_pos[i] = writer.WriteEmptyOffsetReference(
      ElementType::WaveFile_ChannelInfo, true
    );
  }

  std::vector<std::size_t> samples_offset_pos(m_channel_infos.size());
  std::vector<std::size_t> dsp_adpcm_offset_pos(m_channel_infos.size());
  std::vector<std::size_t> channel_infos_start(m_channel_infos.size());
  for (uint i {0}; i < m_channel_infos.size(); ++i) {
    // Channel Info Bin
    writer.WriteCurrentOffsetAt<std::int32_t>(channel_infos_offset_pos[i], channel_info_ref_table_start);
    
    channel_infos_start[i] = writer.Tell();
    samples_offset_pos[i] = writer.WriteEmptyOffsetReference(
      ElementType::General_ByteStream, true
    );
    dsp_adpcm_offset_pos[i] = writer.WriteEmptyOffsetReference(
      ElementType::Codec_DspAdpcmInfo, true
    );
    writer.Write<std::uint32_t>(0); // reserved
  }

  for (uint i {0}; i < m_channel_infos.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::int32_t>(dsp_adpcm_offset_pos[i], channel_infos_start[i]);
    writer.Write(m_channel_infos[i].adpcm_info);
    writer.Write<std::uint16_t>(0); // padding
  }

  writer.AlignUp(GetAlignment(writer.Endian()));
  
  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, section_start);

  return samples_offset_pos;
}

void Fwav::SerializeDataBlock(util::AudioWriter& writer,
                              const std::vector<std::size_t>& samples_offset_pos) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(DataMagic);
  std::size_t section_size_pos {writer.WritePendingValue()};

  std::size_t data_start {writer.Tell()};

  writer.AlignUp(GetAlignment(writer.Endian()));
  
  for (uint i {0}; i < samples_offset_pos.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::int32_t>(samples_offset_pos[i], data_start);
    writer.WriteSamples(m_samples[i], m_encoding);

    if (i != samples_offset_pos.size() - 1)
      writer.AlignUp(GetAlignment(writer.Endian()));
  }

  // Add padding so the file has the correct file size when reserializing
  while (writer.Tell() % 8 != 0)
    writer.Write<std::uint8_t>(0);

  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, section_start);
}
} // namespace oead::audio::fwav