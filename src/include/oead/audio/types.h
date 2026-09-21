#pragma once

#include <variant>
#include <vector>

#include "oead/audio/element_type.h"
#include "oead/types.h"

namespace oead::audio {
// Reference (nn::atk::detail::Util::Reference)
struct Reference {
  ElementType type_id {ElementType::Blank};
  s32 offset {0};

  OEAD_DEFINE_FIELDS(Reference, type_id, offset);
};

// ReferenceWithSize (nn::atk::detail::Util::ReferenceWithSize)
struct SizedReference {
  u16 type_id;
  s32 offset;
  u32 size;

  OEAD_DEFINE_FIELDS(SizedReference, type_id, offset, size);
};

// Table (nn::atk::detail::Util::Table)
template <typename T>
struct Table {
  u32 count;
  std::vector<T> items;
};

// ReferenceTable (nn::atk::detail::Util::ReferenceTable)
struct ReferenceTable : Table<Reference> {};

using Sample = std::variant<std::int8_t, std::int16_t, std::uint8_t, s32>;
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
  u16 byte_order_mark;
  u16 head_size;
  u32 version;
  u32 file_size;
  u16 block_count;
  u16 reserved;
  std::vector<SizedReference> block_refs;
};

struct BlockHeader {
  std::array<char, 4> signature;
  u32 section_size;

  OEAD_DEFINE_FIELDS(BlockHeader, signature, section_size);
};
} // namespace oead::audio
