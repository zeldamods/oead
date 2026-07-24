#include <oead/audio/fstp.h>

#include "oead/errors.h"
#include "oead/util/magic_utils.h"

namespace oead::audio::fstp {
constexpr auto FstpMagic = util::MakeMagic("FSTP");
constexpr auto PdatMagic = util::MakeMagic("PDAT");

Fstp::Fstp(tcb::span<const u8> data) {
  util::AudioReader reader {data, util::Endianness::Little};
}

void Fstp::Deserialize(util::AudioReader& reader) {
  std::size_t file_start {reader.Tell()};

  auto header = reader.ReadSoundFileHeader();

  if (header.signature != FstpMagic)
    throw InvalidDataError("Invalid FSTP magic");
  
  m_version = header.version;
  if (m_version > 0x20100)
    throw InvalidDataError("Unsupported FSTP version");

  m_endian = reader.Endian();

  for (auto& ref : header.block_refs) {
    reader.Seek(file_start + ref.offset);

    auto element_type = static_cast<ElementType>(ref.type_id);

    if (element_type == ElementType::StreamSoundFile_InfoBlock) {
      m_info = {reader};
    }
    else if (element_type == ElementType::StreamSoundFile_PrefetchDataBlock) {
      reader.Read<BlockHeader>();

      std::size_t table_start {reader.Tell()};
      auto prefetch_data_table {reader.ReadTable<PrefetchData>()};

      m_data.prefetch_metadata.resize(prefetch_data_table.count);
      m_data.sample_data.resize(prefetch_data_table.count);

      for (uint i {0}; i < prefetch_data_table.count; ++i) {
        m_data.prefetch_metadata[i] = prefetch_data_table.items[i];
        std::size_t offset {table_start + sizeof(std::uint32_t) + 
                            m_data.prefetch_metadata[i].to_prefetch_samples.offset};
        
        reader.Seek(offset);
        m_data.sample_data[i].resize(m_data.prefetch_metadata[i].prefetch_size);
        for (auto& data : m_data.sample_data[i])
          data = reader.Read<std::uint8_t>();
      }

    }
  }
}

std::vector<u8> Fstp::ToBinary() const {
  util::AudioWriter writer {m_endian};
  Serialize(writer);
  return writer.Finalize();
}

void Fstp::Serialize(util::AudioWriter& writer) const {
  std::size_t file_start {writer.Tell()};

  std::map<std::string, std::vector<std::size_t>> pending_header_values {writer.WriteSoundFileHeader(
    FstpMagic, 
    m_version, 
    {
      ElementType::StreamSoundFile_InfoBlock,
      ElementType::StreamSoundFile_PrefetchDataBlock,
    }
  )};

  // INFO
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][0], 
                                            file_start);
  std::size_t info_section_start {writer.Tell()};
  m_info.Serialize(writer);
  writer.WriteCurrentOffsetAt<std::uint32_t>(pending_header_values["blocks_size"][0], 
                                             info_section_start);
  
  // PDAT
  writer.WriteCurrentOffsetAt<std::int32_t>(pending_header_values["blocks"][1], 
                                            file_start);
  std::size_t pdat_section_start {writer.Tell()};
  SerializeDataBlock(writer);
  writer.WriteCurrentOffsetAt<std::uint32_t>(pending_header_values["blocks_size"][1], 
                                             pdat_section_start);

  writer.WriteCurrentOffsetAt<std::uint32_t>(pending_header_values["file_size"][0], 
                                             file_start);
}

void Fstp::SerializeDataBlock(util::AudioWriter& writer) const {
  std::size_t section_start {writer.Tell()};

  writer.Write(PdatMagic);

  // Do size calculations later 
  std::size_t section_size_pos {writer.WritePendingValue()};

  std::vector<std::size_t> sample_data_offsets_pos(m_data.prefetch_metadata.size());
  
  writer.Write<std::uint32_t>(m_data.prefetch_metadata.size());
  std::size_t pdat_table_start {writer.Tell()};
  for (uint i {0}; i < m_data.prefetch_metadata.size(); ++i) {
    writer.Write(m_data.prefetch_metadata[i].start_frame);
    writer.Write(m_data.prefetch_metadata[i].prefetch_size);
    writer.Write(0); // reserved

    sample_data_offsets_pos[i] = writer.WriteEmptyOffsetReference(ElementType::Blank, true);
  }

  writer.AlignUp(GetAlignment(writer.Endian()));

  for (uint i {0}; i < m_data.sample_data.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::int32_t>(sample_data_offsets_pos[i], pdat_table_start);

    for (auto& data : m_data.sample_data[i])
      writer.Write(data);
  }

  writer.WriteCurrentOffsetAt<std::uint32_t>(section_size_pos, section_start);
}
} // namespeace oead::audio::fstp