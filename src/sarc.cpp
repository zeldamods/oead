/**
 * Copyright (C) 2020 leoetlino
 *
 * This file is part of oead.
 *
 * oead is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * oead is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with oead.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/numbers.h>
#include <array>
#include <numeric>

#include <cmrc/cmrc.hpp>
#include <ryml.hpp>

#include <oead/errors.h>
#include <oead/sarc.h>
#include <oead/util/align.h>
#include <oead/util/magic_utils.h>
#include <oead/util/string_utils.h>
#include "yaml.h"

CMRC_DECLARE(oead::res);

namespace oead {

namespace sarc {

constexpr auto SarcMagic = util::MakeMagic("SARC");
constexpr auto SfatMagic = util::MakeMagic("SFAT");
constexpr auto SfntMagic = util::MakeMagic("SFNT");

struct ResHeader {
  std::array<char, 4> magic;
  u16 header_size;
  u16 bom;
  u32 file_size;
  u32 data_offset;
  u16 version;
  u16 reserved;
  OEAD_DEFINE_FIELDS(ResHeader, magic, header_size, bom, file_size, data_offset, version, reserved);
};
static_assert(sizeof(ResHeader) == 0x14);

struct ResFatHeader {
  std::array<char, 4> magic;
  u16 header_size;
  u16 num_files;
  u32 hash_multiplier;
  OEAD_DEFINE_FIELDS(ResFatHeader, magic, header_size, num_files, hash_multiplier);
};
static_assert(sizeof(ResFatHeader) == 0xC);

struct ResFatEntry {
  u32 name_hash;
  u32 rel_name_optional_offset;
  /// Relative to the data offset.
  u32 data_begin;
  /// Relative to the data offset.
  u32 data_end;

  OEAD_DEFINE_FIELDS(ResFatEntry, name_hash, rel_name_optional_offset, data_begin, data_end);
};

struct ResFntHeader {
  std::array<char, 4> magic;
  u16 header_size;
  u16 reserved;
  OEAD_DEFINE_FIELDS(ResFntHeader, magic, header_size, reserved);
};
static_assert(sizeof(ResFntHeader) == 0x8);

constexpr u32 HashName(u32 multiplier, std::string_view name) {
  u32 hash = 0;
  for (const char c : name)
    hash = hash * multiplier + c;
  return hash;
}

}  // namespace sarc

// Note: This mirrors what sead::SharcArchiveRes::prepareArchive_ does.
Sarc::Sarc(tcb::span<const u8> data) : m_reader{data, util::Endianness::Big} {
  m_reader = {data, util::ByteOrderMarkToEndianness(m_reader.Read<sarc::ResHeader>().value().bom)};
  const auto header = *m_reader.Read<sarc::ResHeader>();

  if (header.magic != sarc::SarcMagic)
    throw InvalidDataError("Invalid SARC magic");
  if (header.version != 0x0100)
    throw InvalidDataError("Unknown SARC version");
  if (header.header_size != sizeof(sarc::ResHeader))
    throw InvalidDataError("Invalid SARC header size");

  const auto fat_header = m_reader.Read<sarc::ResFatHeader>().value();
  if (fat_header.magic != sarc::SfatMagic)
    throw InvalidDataError("Invalid SFAT magic");
  if (fat_header.header_size != sizeof(sarc::ResFatHeader))
    throw InvalidDataError("Invalid SFAT header size");
  if (fat_header.num_files >> 0xE)
    throw InvalidDataError("Too many files");

  m_num_files = fat_header.num_files;
  m_entries_offset = m_reader.Tell();
  m_hash_multiplier = fat_header.hash_multiplier;
  m_data_offset = header.data_offset;

  const auto fnt_header_offset = m_entries_offset + sizeof(sarc::ResFatEntry) * m_num_files;
  const auto fnt_header = m_reader.Read<sarc::ResFntHeader>(fnt_header_offset).value();
  if (fnt_header.magic != sarc::SfntMagic)
    throw InvalidDataError("Invalid SFNT magic");
  if (fnt_header.header_size != sizeof(sarc::ResFntHeader))
    throw InvalidDataError("Invalid SFNT header size");

  m_names_offset = m_reader.Tell();
  if (m_data_offset < m_names_offset)
    throw InvalidDataError("File data should not be stored before the name table");
}

Sarc::File Sarc::GetFile(u16 index) const {
  if (index > m_num_files)
    throw std::out_of_range("Sarc::GetFile: out of range: " + std::to_string(index));

  const auto entry_offset = m_entries_offset + sizeof(sarc::ResFatEntry) * index;
  const auto entry = m_reader.Read<sarc::ResFatEntry>(entry_offset).value();

  File file{};
  if (entry.rel_name_optional_offset) {
    const auto name_offset = m_names_offset + (entry.rel_name_optional_offset & 0xFFFFFF) * 4;
    file.name = m_reader.ReadString<std::string_view>(name_offset);
  }
  file.data =
      m_reader.span().subspan(m_data_offset + entry.data_begin, entry.data_end - entry.data_begin);
  return file;
}

std::optional<Sarc::File> Sarc::GetFile(std::string_view name) const {
  if (m_num_files == 0)
    return std::nullopt;

  const auto wanted_hash = sarc::HashName(m_hash_multiplier, name);

  // Perform a binary search.
  u32 a = 0;
  u32 b = m_num_files - 1;
  while (a <= b) {
    const u32 m = (a + b) / 2;
    const auto hash = m_reader.Read<u32>(m_entries_offset + sizeof(sarc::ResFatEntry) * m);
    if (wanted_hash < hash)
      b = m - 1;
    else if (wanted_hash > hash)
      a = m + 1;
    else
      return GetFile(m);
  }
  return std::nullopt;
}

bool Sarc::operator==(const Sarc& other) const {
  return absl::c_equal(m_reader.span(), other.m_reader.span());
}

bool Sarc::AreFilesEqual(const Sarc& other) const {
  if (GetNumFiles() != other.GetNumFiles())
    return false;

  for (const auto& [file1, file2] : easy_iterator::zip(GetFiles(), other.GetFiles())) {
    if (file1 != file2)
      return false;
  }
  return true;
}

static constexpr bool IsValidAlignment(size_t alignment) {
  return alignment != 0 && (alignment & (alignment - 1)) == 0;
}

size_t Sarc::GuessMinAlignment() const {
  static constexpr size_t MinAlignment = 4;
  size_t gcd = MinAlignment;
  for (size_t i = 0; i < m_num_files; ++i) {
    const u32 entry_offset = m_entries_offset + sizeof(sarc::ResFatEntry) * i;
    const u32 data_begin = m_reader.Read<sarc::ResFatEntry>(entry_offset).value().data_begin;
    gcd = std::gcd(gcd, m_data_offset + data_begin);
  }

  // If the GCD is not a power of 2, the files are most likely not aligned.
  if (!IsValidAlignment(gcd))
    return MinAlignment;

  return gcd;
}

static auto& GetBotwFactoryNames() {
  static auto names = [] {
    absl::flat_hash_set<std::string_view> names;
    const auto fs = cmrc::oead::res::get_filesystem();
    const auto info_tsv_file = fs.open("data/botw_resource_factory_info.tsv");
    util::SplitStringByLine({info_tsv_file.begin(), info_tsv_file.size()},
                            [&names](std::string_view line) {
                              const auto tab_pos = line.find('\t');
                              names.emplace(line.substr(0, tab_pos));
                            });
    return names;
  }();
  return names;
}

static const auto& GetAglEnvAlignmentRequirements() {
  static auto requirements = [] {
    std::vector<std::pair<std::string, u32>> requirements;

    const auto fs = cmrc::oead::res::get_filesystem();
    const auto info_tsv_file = fs.open("data/aglenv_file_info.json");

    yml::InitRymlIfNeeded();
    const auto tree =
        ryml::parse(yml::StrViewToRymlSubstr({info_tsv_file.begin(), info_tsv_file.size()}));

    for (const ryml::NodeRef& entry : tree.rootref()) {
      int alignment = 1;
      if (absl::SimpleAtoi(yml::RymlSubstrToStrView(entry["align"].val()), &alignment)) {
        requirements.emplace_back(std::string(yml::RymlSubstrToStrView(entry["ext"].val())),
                                  std::abs(alignment));
        requirements.emplace_back(std::string(yml::RymlSubstrToStrView(entry["bext"].val())),
                                  std::abs(alignment));
      }
    }
    return requirements;
  }();
  return requirements;
}

void SarcWriter::AddDefaultAlignmentRequirements() {
  for (const auto& [type, alignment] : GetAglEnvAlignmentRequirements())
    AddAlignmentRequirement(type, alignment);

  // BotW: Pack/Bootup.pack/Env/env.sgenvb/postfx/*.bksky (AAMP)
  AddAlignmentRequirement("ksky", 8);
  AddAlignmentRequirement("bksky", 8);
  // BotW: Pack/TitleBG.pack/Terrain/System/tera_resource.Nin_NX_NVN.release.ssarc
  AddAlignmentRequirement("gtx", 0x2000);
  AddAlignmentRequirement("sharcb", 0x1000);
  AddAlignmentRequirement("sharc", 0x1000);
  // BotW: Pack/Bootup.pack/Layout/MultiFilter.ssarc/*.baglmf (AAMP)
  AddAlignmentRequirement("baglmf", 0x80);

  // BotW: Font/*.bfarc/.bffnt
  AddAlignmentRequirement("bffnt", m_endian == util::Endianness::Big ? 0x2000 : 0x1000);
}

std::pair<u32, std::vector<u8>> SarcWriter::Write() {
  util::BinaryWriter writer{m_endian};

  writer.Seek(sizeof(sarc::ResHeader));

  // Sort the files by name hash to make binary searches possible.
  std::vector<std::reference_wrapper<FileMap::value_type>> files{m_files.begin(), m_files.end()};
  absl::c_sort(files, [this](const FileMap::value_type& a, const FileMap::value_type& b) {
    return sarc::HashName(m_hash_multiplier, a.first) < sarc::HashName(m_hash_multiplier, b.first);
  });

  // Try to avoid unnecessary reallocations by making the buffer large enough to fit the whole SARC.
  size_t estimated_size =
      sizeof(sarc::ResHeader) + sizeof(sarc::ResFatHeader) + sizeof(sarc::ResFntHeader);
  for (const FileMap::value_type& pair : files) {
    const auto& [name, data] = pair;
    estimated_size += sizeof(sarc::ResFatEntry);
    estimated_size += util::AlignUp(name.size() + 1, 4);
    estimated_size += data.size();
  }
  writer.Buffer().reserve(1.5f * estimated_size);

  // SFAT
  sarc::ResFatHeader fat_header{};
  fat_header.magic = sarc::SfatMagic;
  fat_header.header_size = sizeof(fat_header);
  fat_header.num_files = u16(m_files.size());
  fat_header.hash_multiplier = m_hash_multiplier;
  writer.Write(fat_header);

  AddDefaultAlignmentRequirements();
  std::vector<u32> alignments;
  alignments.reserve(files.size());

  {
    u32 rel_string_offset = 0;
    u32 rel_data_offset = 0;
    for (const FileMap::value_type& pair : files) {
      const auto& [name, data] = pair;
      const u32 alignment = GetAlignmentForFile(name, data);
      alignments.emplace_back(alignment);

      sarc::ResFatEntry entry{};
      entry.name_hash = sarc::HashName(m_hash_multiplier, name);
      entry.rel_name_optional_offset = 1 << 24 | (rel_string_offset / 4);
      entry.data_begin = util::AlignUp(rel_data_offset, alignment);
      entry.data_end = entry.data_begin + data.size();
      writer.Write(entry);

      rel_data_offset = entry.data_end;
      rel_string_offset += util::AlignUp(name.size() + 1, 4);
    }
  }

  // SFNT
  sarc::ResFntHeader fnt_header{};
  fnt_header.magic = sarc::SfntMagic;
  fnt_header.header_size = sizeof(fnt_header);
  writer.Write(fnt_header);
  for (const FileMap::value_type& pair : files) {
    writer.WriteCStr(pair.first);
    writer.AlignUp(4);
  }

  // File data
  const u32 required_alignment = absl::c_accumulate(alignments, 1u, std::lcm<u32, u32>);
  writer.AlignUp(required_alignment);
  const u32 data_offset_begin = u32(writer.Tell());
  for (const auto& [pair, alignment] : easy_iterator::zip(files, alignments)) {
    writer.AlignUp(alignment);
    writer.WriteBytes(pair.second);
  }

  sarc::ResHeader header{};
  header.magic = sarc::SarcMagic;
  header.header_size = sizeof(header);
  header.bom = 0xFEFF;
  header.file_size = writer.Tell();
  header.data_offset = data_offset_begin;
  header.version = 0x0100;
  writer.Seek(0);
  writer.Write(header);
  return {required_alignment, writer.Finalize()};
}

void SarcWriter::SetMinAlignment(size_t alignment) {
  if (!IsValidAlignment(alignment))
    throw std::invalid_argument("Invalid alignment");
  m_min_alignment = alignment;
}

void SarcWriter::AddAlignmentRequirement(std::string extension, size_t alignment) {
  if (!IsValidAlignment(alignment))
    throw std::invalid_argument("Invalid alignment");
  m_alignment_map.insert_or_assign(std::move(extension), alignment);
}

static bool IsSarc(tcb::span<const u8> data) {
  return data.size() >= 0x20 && (std::memcmp(data.data(), "SARC", 4) == 0 ||
                                 (std::memcmp(data.data(), "Yaz0", 4) == 0 &&
                                  std::memcmp(data.data() + 0x11, "SARC", 4) == 0));
}

/// Detects alignment requirements for binary files that use nn::util::BinaryFileHeader.
static u32 GetAlignmentForNewBinaryFile(tcb::span<const u8> data) {
  util::BinaryReader reader{data, util::Endianness::Big};
  if (data.size() <= 0x20)
    return 1;

  const auto bom = *reader.Read<u16>(0xC);
  if (bom != 0xFEFF && bom != 0xFFFE)
    return 1;
  reader.SetEndian(util::ByteOrderMarkToEndianness(bom));

  // If the file size doesn't match, then the file likely does not contain a
  // nn::util::BinaryFileHeader
  const auto file_size = *reader.Read<u32>(0x1C);
  if (file_size != data.size())
    return 1;

  return 1 << data[0xE];
}

static u32 GetAlignmentForCafeBflim(tcb::span<const u8> data) {
  if (data.size() <= 0x28 || std::memcmp(data.data() + data.size() - 0x28, "FLIM", 4) != 0)
    return 1;

  return util::SwapIfNeeded(u16(util::BitCastPtr<u16>(data.data() + data.size() - 0x8)),
                            util::Endianness::Big);
}

u32 SarcWriter::GetAlignmentForFile(std::string_view name, tcb::span<const u8> data) const {
  const std::string_view::size_type dot_pos = name.rfind('.');
  const std::string_view ext = dot_pos + 1 < name.size() ? name.substr(dot_pos + 1) : "";

  u32 alignment = m_min_alignment;

  if (const auto it = m_alignment_map.find(ext); it != m_alignment_map.end()) {
    alignment = std::lcm(alignment, it->second);
  }

  // In some archives (SMO's for example), Nintendo seems to use a somewhat arbitrary
  // alignment requirement (0x2000) for nested SARCs.
  if (m_mode == Mode::Legacy && IsSarc(data)) {
    alignment = std::lcm(alignment, 0x2000u);
  }

  // For resources that are unhandled by a BotW-style resource system, or for resources
  // from games that do not have such a system, try to detect the alignment.
  if (m_mode == Mode::Legacy || !GetBotwFactoryNames().contains(ext)) {
    alignment = std::lcm(alignment, GetAlignmentForNewBinaryFile(data));
    if (m_endian == util::Endianness::Big)
      alignment = std::lcm(alignment, GetAlignmentForCafeBflim(data));
  }

  return alignment;
}

SarcWriter SarcWriter::FromSarc(const Sarc& archive) {
  SarcWriter writer;
  writer.SetEndianness(archive.GetEndianness());
  writer.SetMinAlignment(archive.GuessMinAlignment());
  writer.m_files.reserve(archive.GetNumFiles());
  for (const Sarc::File& file : archive.GetFiles()) {
    writer.m_files.emplace(std::string(file.name),
                           std::vector<u8>(file.data.begin(), file.data.end()));
  }
  return writer;
}

}  // namespace oead
