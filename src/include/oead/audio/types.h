#pragma once

#include <variant>
#include <vector>

#include "oead/types.h"

namespace oead::audio {
// Reference (nn::atk::detail::Util::Reference)
struct Reference {
  std::uint16_t type_id;
  std::int32_t offset;

  OEAD_DEFINE_FIELDS(Reference, type_id, offset);
};

// ReferenceWithSize (nn::atk::detail::Util::ReferenceWithSize)
struct SizedReference {
  std::uint16_t type_id;
  std::int32_t offset;
  std::uint32_t size;

  OEAD_DEFINE_FIELDS(SizedReference, type_id, offset, size);
};

// Table (nn::atk::detail::Util::Table)
template <typename T>
struct Table {
  std::uint32_t count;
  std::vector<T> items;
};

// ReferenceTable (nn::atk::detail::Util::ReferenceTable)
struct ReferenceTable : Table<Reference> {};

using Sample = std::variant<std::int8_t, std::int16_t, std::uint8_t, std::int32_t>;
using Channel = std::vector<Sample>;

// ReferenceTable (nn::atk::detail::Util::SampleFormat)
enum class SampleFormat : std::uint8_t {
  PCMS8,
  PCMS16,
  DSPADPCM,
  PCMS32,
};

// AssetType (aal::AssetType)
enum class AssetType : std::uint8_t {
  Wave,
  Stream,
  Unknown
};

struct SoundFileHeader {
  std::array<char, 4> signature;
  std::uint16_t byte_order_mark;
  std::uint16_t head_size;
  std::uint32_t version;
  std::uint32_t file_size;
  std::uint16_t block_count;
  std::uint16_t reserved;
  std::vector<SizedReference> block_refs;
};

struct BlockHeader {
  std::array<char, 4> signature;
  uint32_t section_size;

  OEAD_DEFINE_FIELDS(BlockHeader, signature, section_size);
};
} // namespace oead::audio