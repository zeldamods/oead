#include <oead/audio/amta.h>

#include <algorithm>

#include "oead/errors.h"
#include "oead/util/magic_utils.h"

namespace oead::audio::amta {
constexpr auto AmtaMagic = util::MakeMagic("AMTA");

Amta::Amta(tcb::span<const u8> data) {
  util::AudioReader reader {data, util::Endianness::Little};
  Deserialize(reader);
}

void Amta::Deserialize(util::AudioReader& reader) {
  reader.MarkSectionStart();

  auto header {reader.Read<AudioMetaHeader>()};

  if (util::ByteOrderMarkToEndianness(header.bom) == util::Endianness::Little) {
    reader.SwapEndianness();
    reader.SectionSeek(0);
    header = reader.Read<AudioMetaHeader>();
  }

  if (header.signature != AmtaMagic)
    throw InvalidDataError("Invalid AMTA magic");

  if (header.version > 0x400)
    throw InvalidDataError("Unsupported AMTA version");

  m_version = header.version;
  m_endianness = reader.Endian();

  // DATA
  reader.SectionSeek(header.data_offset);
  reader.Read<BlockHeader>();
  auto asset_name_offset = reader.Read<u32>();
  m_sample_count = reader.Read<u32>();
  m_asset_type = reader.Read<AssetType>();
  m_channel_count = reader.Read<u8>();
  m_used_stream_tracks = reader.Read<u8>();
  m_flags = reader.Read<u8>();
  m_duration = reader.Read<u32>();
  m_sample_rate = reader.Read<u32>();
  m_loop_start_frame = reader.Read<u32>();
  m_loop_end_frame = reader.Read<u32>();
  m_loudness = reader.Read<float>();

  for (auto& track : m_stream_tracks)
    track = reader.Read<StreamTrack>();

  if (m_version >= 0x400)
    m_amplitude_peak = reader.Read<float>();

  // MARK
  reader.SectionSeek(header.mark_offset);
  DeserializeMarkerBlock(reader, header.strg_offset);

  // EXT_
  reader.SectionSeek(header.ext_offset);
  DeserializeExtBlock(reader, header.strg_offset);

  m_asset_name = reader.ReadString(reader.SectionStart() + header.strg_offset + 
                                   sizeof(BlockHeader) + asset_name_offset);
}

void Amta::DeserializeMarkerBlock(util::AudioReader& reader, u32 to_string_table) {
  reader.Read<BlockHeader>();  
  auto marker_info_count {reader.Read<std::uint32_t>()};

  m_markers.resize(marker_info_count);
  for (auto& marker : m_markers) {
    marker.id = reader.Read<std::uint32_t>();
    auto name_offset {reader.Read<std::uint32_t>()};
    marker.start_pos = reader.Read<std::uint32_t>();
    marker.length = reader.Read<std::uint32_t>();

    marker.name = reader.ReadString(reader.SectionStart() + to_string_table + 
                                    sizeof(BlockHeader) + name_offset);
    marker.name.resize(std::max(marker.name.size() + 1, 2UL));
  }
}

void Amta::DeserializeExtBlock(util::AudioReader& reader, u32 to_string_table) {
  reader.Read<BlockHeader>();
  auto ext_count {reader.Read<std::uint32_t>()};
  
  m_ext_entries.resize(ext_count);
  for (auto& entry : m_ext_entries) {
    auto name_offset {reader.Read<std::uint32_t>()};
    entry.value = reader.Read<std::uint32_t>();

    entry.name = reader.ReadString(reader.SectionStart() + to_string_table + 
                                   sizeof(BlockHeader) + name_offset);
  }
}

std::vector<u8> Amta::ToBinary() const {
  util::AudioWriter writer {m_endianness};
  Serialize(writer);
  return writer.Finalize();
}

void Amta::Serialize(util::AudioWriter& writer) const {
  std::size_t file_start{writer.Tell()};

  writer.WriteString("AMTA");
  writer.Write<std::uint16_t>(0xFEFF);
  writer.Write(m_version);

  std::size_t file_size_pos {writer.WritePendingValue<std::uint32_t>()};
  std::size_t data_offset_pos {writer.WritePendingValue<std::uint32_t>()};
  std::size_t mark_offset_pos {writer.WritePendingValue<std::uint32_t>()};
  std::size_t ext_offset_pos {writer.WritePendingValue<std::uint32_t>()};
  std::size_t strg_offset_pos {writer.WritePendingValue<std::uint32_t>()};

  writer.WriteCurrentOffsetAt<std::int32_t>(data_offset_pos, file_start);
  SerializeData(writer);

  writer.WriteCurrentOffsetAt<std::int32_t>(mark_offset_pos, file_start);
  std::vector<std::size_t> marker_name_offsets {SerializeMarker(writer)};

  writer.WriteCurrentOffsetAt<std::int32_t>(ext_offset_pos, file_start);
  std::vector<std::size_t> ext_name_offsets {SerializeExt(writer)};

  writer.WriteCurrentOffsetAt<std::int32_t>(strg_offset_pos, file_start);
  SerializeStringTable(writer, marker_name_offsets, ext_name_offsets);

  while (writer.Tell() % 4 != 0)
      writer.Write<std::uint8_t>(0);

  writer.WriteCurrentOffsetAt<std::int32_t>(file_size_pos, file_start);
}

void Amta::SerializeData(util::AudioWriter& writer) const {
  writer.WriteString("DATA");
  writer.Write<uint32_t>(sizeof(AudioMetaDataBin) - sizeof(BlockHeader));

  writer.Write<std::uint32_t>(0);
  writer.Write(m_sample_count);
  writer.Write(m_asset_type);
  writer.Write(m_channel_count);
  writer.Write(m_used_stream_tracks);
  writer.Write(m_flags);
  writer.Write(m_duration);
  writer.Write(m_sample_rate);
  writer.Write(m_loop_start_frame);
  writer.Write(m_loop_end_frame);
  writer.Write(m_loudness);
  
  for (auto& track : m_stream_tracks)
    writer.Write(track);

  if (m_version >= 0x0400)
    writer.Write(m_amplitude_peak);
}

std::vector<std::size_t> Amta::SerializeMarker(util::AudioWriter& writer) const {
  writer.WriteString("MARK");

  // Size of entry_count + size of all entries
  std::size_t section_size {sizeof(std::uint32_t) + sizeof(MarkerInfoBin) * m_markers.size()};
  writer.Write<std::uint32_t>(section_size);

  writer.Write<std::uint32_t>(m_markers.size());

  std::vector<std::size_t> marker_name_offsets(m_markers.size());

  for (uint i {0}; i < m_markers.size(); ++i) {
    writer.Write<std::uint32_t>(m_markers[i].id);

    marker_name_offsets[i] = writer.Tell();
    writer.Write<std::uint32_t>(0);

    writer.Write<std::uint32_t>(m_markers[i].start_pos);
    writer.Write<std::uint32_t>(m_markers[i].length);
  }

  return marker_name_offsets;
}

std::vector<std::size_t> Amta::SerializeExt(util::AudioWriter& writer) const {
  writer.WriteString("EXT_");

  // Size of entry_count + size of all entries
  std::size_t section_size {sizeof(std::uint32_t) + sizeof(ExtEntryBin) * m_ext_entries.size()};
  writer.Write<std::uint32_t>(section_size);

  writer.Write<std::uint32_t>(m_ext_entries.size());
  
  std::vector<std::size_t> ext_name_offsets(m_ext_entries.size());

  for (uint i {0}; i < m_ext_entries.size(); ++i) {
    ext_name_offsets[i] = writer.Tell();
    writer.Write<std::uint32_t>(0);
    writer.Write(m_ext_entries[i].value);
  }

  return ext_name_offsets;
}

void Amta::SerializeStringTable(util::AudioWriter& writer, 
                                std::vector<std::size_t> marker_offsets, 
                                std::vector<std::size_t> ext_offsets) const {
  writer.WriteString("STRG");

  std::size_t section_size_pos {writer.WritePendingValue()};

  std::size_t string_entries_start {writer.Tell()};
  
  writer.WriteCString(m_asset_name);

  for (uint i {0}; i < m_markers.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::int32_t>(marker_offsets[i], string_entries_start);
    writer.WriteCString(m_markers[i].name);
  }

  for (uint i {0}; i < m_ext_entries.size(); ++i) {
    writer.WriteCurrentOffsetAt<std::int32_t>(ext_offsets[i], string_entries_start);
    writer.WriteCString(m_ext_entries[i].name);
  }

  writer.WriteCurrentOffsetAt<std::int32_t>(section_size_pos, string_entries_start);
}
} // namespace oead::audio::meta