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

#pragma once

#include <absl/algorithm/container.h>
#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>
#include <easy_iterator.h>
#include <nonstd/span.h>
#include <optional>
#include <string>
#include <string_view>

#include <oead/types.h>
#include <oead/util/binary_reader.h>

namespace oead {

/// A simple SARC archive reader.
class Sarc {
public:
  /// A file that is stored in a SARC archive.
  struct File {
    /// File name. May be empty for file entries that do not use the file name table.
    std::string_view name;
    /// File data (as a view).
    tcb::span<const u8> data;

    bool operator==(const File& other) const {
      return name == other.name && absl::c_equal(data, other.data);
    }

    bool operator!=(const File& other) const { return !(*this == other); }
  };

  /// File iterator.
  class FileIterator : public easy_iterator::InitializedIterable {
  public:
    FileIterator(u16 index, const Sarc& parent) : m_index{index}, m_parent{parent} {}
    File value() { return m_parent.GetFile(m_index); }
    bool advance() { return ++m_index < m_parent.GetNumFiles(); }
    bool init() { return m_index != m_parent.GetNumFiles(); }

  private:
    u16 m_index;
    const Sarc& m_parent;
  };

  Sarc(tcb::span<const u8> data);

  /// Get the number of files that are stored in the archive.
  u16 GetNumFiles() const { return m_num_files; }
  /// Get the offset to the beginning of file data.
  u32 GetDataOffset() const { return m_data_offset; }
  /// Get the archive endianness.
  util::Endianness GetEndianness() const { return m_reader.Endian(); }

  /// Get a file by name.
  std::optional<File> GetFile(std::string_view name) const;
  /// Get a file by index. Throws if index >= m_num_files.
  File GetFile(u16 index) const;
  /// Returns an iterator over the contained files.
  auto GetFiles() const { return easy_iterator::MakeIterable<FileIterator>(0, *this); }

  /// Guess the minimum data alignment for files that are stored in the archive.
  size_t GuessMinAlignment() const;

  /// Returns true if and only if the raw archive data is identical.
  bool operator==(const Sarc& other) const;
  bool operator!=(const Sarc& other) const { return !(*this == other); }

  bool AreFilesEqual(const Sarc& other) const;

private:
  u16 m_num_files;
  u16 m_entries_offset;
  u32 m_hash_multiplier;
  u32 m_data_offset;
  u32 m_names_offset;
  mutable util::BinaryReader m_reader;
};

class SarcWriter {
public:
  enum class Mode {
    /// Used for games with an old-style resource system that requires aligning nested SARCs
    /// and manual alignment of file data in archives.
    Legacy,
    /// Used for games with a new-style resource system that automatically takes care of data
    /// alignment and does not require manual alignment nor nested SARC alignment.
    New,
  };

  SarcWriter(util::Endianness endian = util::Endianness::Little, Mode mode = Mode::New)
      : m_endian{endian}, m_mode{mode} {}

  /// Make a SarcWriter from a SARC archive. The endianness, data alignment and file content will be
  /// copied from the SARC.
  /// @param archive Source archive
  static SarcWriter FromSarc(const Sarc& archive);

  /// Write a SARC archive using the specified endianness.
  /// Default alignment requirements may be automatically added.
  /// \returns the required data alignment and the data.
  std::pair<u32, std::vector<u8>> Write();

  /// Set the endianness.
  void SetEndianness(util::Endianness endian) { m_endian = endian; }

  /// Set the minimum data alignment.
  /// @param alignment Data alignment (must be a power of 2)
  void SetMinAlignment(size_t alignment);
  /// Add or modify a data alignment requirement for a file type.
  /// Set the alignment to 1 to revert.
  /// @param extension_without_dot File extension without the dot (e.g. "bgparamlist")
  /// @param alignment Data alignment (must be a power of 2)
  void AddAlignmentRequirement(std::string extension_without_dot, size_t alignment);

  void SetMode(Mode mode) { m_mode = mode; }

  /// \warning The map type is an implementation detail.
  using FileMap = absl::flat_hash_map<std::string, std::vector<u8>>;
  /// Files to be written.
  FileMap m_files;

private:
  void AddDefaultAlignmentRequirements();
  u32 GetAlignmentForFile(std::string_view name, tcb::span<const u8> data) const;

  util::Endianness m_endian = util::Endianness::Little;
  Mode m_mode = Mode::New;
  /// Multiplier to use for calculating name hashes.
  u32 m_hash_multiplier = 0x65;
  size_t m_min_alignment = 4;
  /// Required data alignment for specific extensions.
  absl::flat_hash_map<std::string, size_t> m_alignment_map;
};

}  // namespace oead
