#include <oead/audio/fstm.h>

#include "oead/errors.h"
#include "oead/util/magic_utils.h"

namespace oead::audio::fstm {
constexpr auto FstmMagic = util::MakeMagic("FSTM");
constexpr auto InfoMagic = util::MakeMagic("INFO");
constexpr auto SeekMagic = util::MakeMagic("SEEK");
constexpr auto RegionMagic = util::MakeMagic("REGN");
constexpr auto DataMagic = util::MakeMagic("DATA");

Fstm::Fstm(tcb::span<const u8> data) {
  util::AudioReader reader {data, util::Endianness::Little};
  Deserialize(reader);
}

void Fstm::Deserialize(util::AudioReader& reader) {
  reader.MarkSectionStart();

  auto header = reader.ReadSoundFileHeader();

  if (header.signature != FstmMagic)
    throw InvalidDataError("Invalid FSTM magic");
  
  m_version = header.version;
  if (m_version > 0x40000)
    throw InvalidDataError("Unsupported FSTM version");

  m_endian = reader.Endian();

  for (auto& ref : header.block_refs) {
    reader.SectionSeek(ref.offset);

    auto element_type = static_cast<ElementType>(ref.type_id);

    switch (element_type) {
    case ElementType::StreamSoundFile_InfoBlock:
      m_info = {reader};
      break;
    case ElementType::StreamSoundFile_SeekBlock:
      reader.Read<BlockHeader>();

      m_seek_infos.resize(m_info.StreamInfo().block_count);
      for (uint block {0}; block < m_info.StreamInfo().block_count; ++block) {
        m_seek_infos[block].resize(m_info.StreamInfo().channel_count);
        for (auto& seek_info : m_seek_infos[block])
          seek_info = reader.Read<SeekInfo>();
      }
      break;
    case ElementType::StreamSoundFile_RegionBlock:
      m_has_region = true;
      reader.Read<BlockHeader>();

      m_region_infos.resize(m_info.StreamInfo().region_count);
      for (auto& info : m_region_infos)
        info = reader.Read<RegionInfo>();
      break;
    case ElementType::StreamSoundFile_DataBlock:
      reader.Read<BlockHeader>();

      m_samples.resize(m_info.StreamInfo().channel_count);
      reader.Align(0x20);

      for (auto& channel : m_samples) {
        channel = reader.ReadSamples(m_info.StreamInfo().frame_count, 
                                     m_info.StreamInfo().encoding, 
                                     false);
        reader.Align(0x20);
      }
      break;
    default:
        break;
    }
  }
}

InfoBlock::InfoBlock(util::AudioReader& reader) {
  reader.Read<BlockHeader>();

  size_t ref_array_start = reader.Tell();

  auto stminfo_ref = reader.Read<Reference>();
  auto track_info_table_ref = reader.Read<Reference>();
  auto channel_info_table_ref = reader.Read<Reference>();

  if (stminfo_ref.offset != -1) {
    reader.Seek(ref_array_start + stminfo_ref.offset);
    m_stream_info = reader.Read<StreamSoundInfo>();
  }

  // TODO: Verify if Track Info deserialization works
  if (track_info_table_ref.offset != -1) {
    reader.Seek(ref_array_start + track_info_table_ref.offset);
    size_t track_info_table_start {reader.Tell()};

    auto track_info_table = reader.ReadTable<Reference>();
    m_track_infos.resize(track_info_table.count);
    for (uint i {0}; i < track_info_table.count; ++i) {
      Reference track_info_ref {track_info_table.items[i]};
      reader.Seek(track_info_table_start + track_info_ref.offset);

      m_track_infos[i] = reader.Read<TrackInfo>();
    }
  }

  if (channel_info_table_ref.offset != -1) {
    reader.Seek(ref_array_start + channel_info_table_ref.offset);
    size_t channel_info_table_start = reader.Tell();

    auto channel_info_table = reader.ReadTable<Reference>();
    m_detail_channel_infos.resize(channel_info_table.count);

    for (uint i {0}; i < channel_info_table.count; ++i) {
      Reference channel_info = channel_info_table.items[i];
      reader.Seek(channel_info_table_start + channel_info.offset);

      size_t current_offset = reader.Tell();
      auto dsp_adpcm_ref = reader.Read<Reference>();

      reader.Seek(current_offset + dsp_adpcm_ref.offset);

      m_detail_channel_infos[i] = reader.Read<DspAdpcmInfo>();
    }
  }
}

std::vector<u8> Fstm::ToBinary() const {
  util::AudioWriter writer {m_endian};
  Serialize(writer);
  return writer.Finalize();
}

void Fstm::Serialize(util::AudioWriter& writer) const {
  std::size_t file_start {writer.Tell()};

  std::map<std::string, std::vector<std::size_t>> pending_header_values {writer.WriteSoundFileHeader(
    FstmMagic, 
    m_version, 
    {
      ElementType::StreamSoundFile_InfoBlock,
      ElementType::StreamSoundFile_SeekBlock,
      m_has_region ? ElementType::StreamSoundFile_RegionBlock : ElementType::Invalid,
      ElementType::StreamSoundFile_DataBlock,
    }
  )};

  // INFO
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][0], 
                                            file_start);
  std::size_t info_section_start {writer.Tell()};
  m_info.Serialize(writer);
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks_size"][0], 
                                            info_section_start);

  // SEEK
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][1], 
                                            file_start);
  std::size_t seek_section_start {writer.Tell()};
  SerializeSeekBlock(writer);
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks_size"][1], 
                                            seek_section_start);

  // REGN
  if (m_has_region) {
    writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][2], 
                                              file_start);
    std::size_t region_section_start {writer.Tell()};
    SerializeRegionBlock(writer);
    writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks_size"][2], 
                                              region_section_start);
  }

  // DATA
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][3], 
                                            file_start);
  std::size_t data_section_start {writer.Tell()};
  SerializeDataBlock(writer);
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks_size"][3], 
                                            data_section_start);
  
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["file_size"][0], 
                                            file_start);
}

void Fstm::SerializeSeekBlock(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(SeekMagic);
  std::size_t section_size_pos {writer.WritePendingValue()};

  for (auto& block : m_seek_infos) {
    for (auto& info : block)
      writer.Write(info);
  }

  writer.AlignUp(0x20);
  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, section_start);
}

void Fstm::SerializeRegionBlock(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(RegionMagic);
  std::size_t section_size_pos {writer.WritePendingValue()};

  for (auto& info : m_region_infos)
    writer.Write(info);

  writer.AlignUp(0x20);
  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, section_start);
}

void Fstm::SerializeDataBlock(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};
  
  writer.Write(DataMagic);
  std::size_t section_size_offset {writer.WritePendingValue()};

  writer.AlignUp(0x20);

  for (auto& channel : m_samples) {
    writer.WriteSamples(channel, m_info.StreamInfo().encoding);
    if (channel != m_samples.back())
      writer.AlignUp(0x20);
  }

  // padding
  while (writer.Tell() % 0x20 != 0)
    writer.Write<std::uint8_t>(0);

  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_offset, section_start);
}

void InfoBlock::Serialize(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(InfoMagic);
  std::size_t section_size_pos {writer.WritePendingValue()};

  std::size_t reference_array_start {writer.Tell()};

  std::size_t strm_info_offset_pos {writer.WriteEmptyOffsetReference(
    ElementType::StreamSoundFile_StreamSoundInfo, true
  )};

  std::size_t track_info_ref_table_offset_pos = writer.WriteEmptyOffsetReference(
    ElementType::Table_ReferenceTable, !m_track_infos.empty()
  );

  std::size_t channel_info_ref_table_offset_pos = writer.WriteEmptyOffsetReference(
    ElementType::Table_ReferenceTable, !m_detail_channel_infos.empty()
  );

  writer.WriteCurrentOffsetAt<std::int32_t>(strm_info_offset_pos, reference_array_start);
  writer.Write(m_stream_info);

  // TODO: Verify if Track Info serialization works
  std::vector<std::size_t> track_infos_offset_pos(m_track_infos.size());
  if (!m_track_infos.empty()) {
    writer.WriteCurrentOffsetAt<std::int32_t>(track_info_ref_table_offset_pos, reference_array_start);
    std::size_t track_info_table_start {writer.Tell()};
    writer.Write<uint32_t>(m_track_infos.size());
    for (uint i {0}; i < m_track_infos.size(); ++i) {
      track_infos_offset_pos[i] = writer.WriteEmptyOffsetReference(
          ElementType::StreamSoundFile_TrackInfo, true
      );
    }

    for (uint i {0}; i < m_track_infos.size(); ++i) {
      writer.WriteCurrentOffsetAt<std::int32_t>(track_infos_offset_pos[i], track_info_table_start);
      writer.Write(m_track_infos[i]);
    }
  }

  std::vector<std::size_t> channel_infos_offset_pos(m_detail_channel_infos.size());
  if (!m_detail_channel_infos.empty()) {
    writer.WriteCurrentOffsetAt<std::int32_t>(channel_info_ref_table_offset_pos, reference_array_start);
    std::size_t channel_info_table_start {writer.Tell()};
    writer.Write<uint32_t>(m_detail_channel_infos.size());
    for (uint i {0}; i < m_detail_channel_infos.size(); ++i) {
      channel_infos_offset_pos[i] = writer.WriteEmptyOffsetReference(
          ElementType::StreamSoundFile_ChannelInfo, true
      );
    }

    std::vector<std::size_t> detail_channel_infos_offset_pos(m_detail_channel_infos.size());
    for (uint i {0}; i < channel_infos_offset_pos.size(); ++i) {
      writer.WriteCurrentOffsetAt<std::int32_t>(channel_infos_offset_pos[i], channel_info_table_start);
      detail_channel_infos_offset_pos[i] = writer.WriteEmptyOffsetReference(
          ElementType::Codec_DspAdpcmInfo, true
      );
    }

    for (uint i {0}; i < m_detail_channel_infos.size(); ++i) {
      writer.WriteCurrentOffsetAt<std::int32_t>(detail_channel_infos_offset_pos[i], detail_channel_infos_offset_pos[i] - sizeof(std::uint32_t));
      writer.Write(m_detail_channel_infos[i]);
      writer.Write<std::uint16_t>(0); // padding?
    }
  }

  writer.AlignUp(0x20);
  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, section_start);
}
} // namespace oead::audio::fstm