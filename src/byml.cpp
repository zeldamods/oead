/**
 * Copyright (C) 2020 leoetlino <leo@leolam.fr>
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

#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <map>
#include <string_view>

#include <oead/byml.h>
#include "util/align.h"
#include "util/binary_reader.h"
#include "util/bit_utils.h"
#include "util/iterator_utils.h"
#include "util/variant_utils.h"

namespace oead {

namespace byml {

struct ResHeader {
  /// “BY” (big endian) or “YB” (little endian).
  std::array<char, 2> magic;
  /// Format version (2 or 3).
  u16 version;
  /// Offset to the hash key table, relative to start (usually 0x010)
  /// May be 0 if no hash nodes are used. Must be a string table node (0xc2).
  u32 hash_key_table_offset;
  /// Offset to the string table, relative to start. May be 0 if no strings are used.
  /// Must be a string table node (0xc2).
  u32 string_table_offset;
  /// Offset to the root node, relative to start. May be 0 if the document is totally empty.
  /// Must be either an array node (0xc0) or a hash node (0xc1).
  u32 root_node_offset;
};
static_assert(sizeof(ResHeader) == 0x10);

enum class NodeType : u8 {
  String = 0xa0,
  Array = 0xc0,
  Hash = 0xc1,
  StringTable = 0xc2,
  Bool = 0xd0,
  Int = 0xd1,
  Float = 0xd2,
  UInt = 0xd3,
  Int64 = 0xd4,
  UInt64 = 0xd5,
  Double = 0xd6,
  Null = 0xff,
};

constexpr NodeType GetNodeType(Byml::Type type) {
  constexpr std::array map{
      NodeType::Null,  NodeType::String, NodeType::Array,  NodeType::Hash,
      NodeType::Bool,  NodeType::Int,    NodeType::Float,  NodeType::UInt,
      NodeType::Int64, NodeType::UInt64, NodeType::Double,
  };
  return map[u8(type)];
}

template <typename T = NodeType>
constexpr bool IsContainerType(T type) {
  return type == T::Array || type == T::Hash;
}

template <typename T = NodeType>
constexpr bool IsLongType(T type) {
  return type == T::Int64 || type == T::UInt64 || type == T::Double;
}

template <typename T = NodeType>
constexpr bool IsNonInlineType(T type) {
  return IsContainerType(type) || IsLongType(type);
}

constexpr bool IsValidVersion(int version) {
  return 2 <= version && version <= 4;
}

class StringTableParser {
public:
  StringTableParser() = default;
  StringTableParser(util::BinaryReader& reader, u32 offset) : m_offset{offset} {
    if (offset == 0)
      return;
    const auto type = reader.Read<NodeType>(offset);
    const auto num_entries = reader.ReadU24();
    if (!type || *type != NodeType::StringTable || !num_entries)
      throw InvalidDataError("Invalid string table");
    m_size = *num_entries;
  }

  std::string GetString(util::BinaryReader& reader, u32 idx) const {
    if (idx >= m_size)
      throw std::out_of_range("Invalid string table entry index");

    const auto rel_offset = reader.Read<u32>(m_offset + 4 + 4 * idx);
    // This is safe even for idx = N - 1 since the offset array has N+1 elements.
    const auto next_rel_offset = reader.Read<u32>();
    if (!rel_offset || !next_rel_offset)
      throw InvalidDataError("Invalid string table: failed to read offsets");
    if (*next_rel_offset < *rel_offset)
      throw InvalidDataError("Invalid string table: inconsistent offsets");

    const size_t max_len = *next_rel_offset - *rel_offset;
    return reader.ReadString(m_offset + *rel_offset, max_len);
  }

private:
  u32 m_offset = 0;
  u32 m_size = 0;
};

class Parser {
public:
  Parser(tcb::span<const u8> data) {
    if (data.size() < sizeof(ResHeader))
      throw InvalidDataError("Invalid header");

    const bool is_big_endian = data[0] == 'B' && data[1] == 'Y';
    const bool is_little_endian = data[0] == 'Y' && data[1] == 'B';
    const auto endianness = is_big_endian ? util::Endianness::Big : util::Endianness::Little;
    if (!is_big_endian && !is_little_endian)
      throw InvalidDataError("Invalid magic");

    m_reader = {data, endianness};

    const u16 version = *m_reader.Read<u16>(offsetof(ResHeader, version));
    if (!IsValidVersion(version))
      throw InvalidDataError("Unexpected version");

    m_hash_key_table = StringTableParser(
        m_reader, *m_reader.Read<u32>(offsetof(ResHeader, hash_key_table_offset)));
    m_string_table =
        StringTableParser(m_reader, *m_reader.Read<u32>(offsetof(ResHeader, string_table_offset)));
    m_root_node_offset = *m_reader.Read<u32>(offsetof(ResHeader, root_node_offset));
  }

  Byml Parse() {
    if (m_root_node_offset == 0)
      return Byml::Null();
    return ParseContainerNode(m_root_node_offset);
  }

private:
  Byml ParseValueNode(u32 offset, NodeType type) {
    const auto raw = m_reader.Read<u32>(offset);
    if (!raw)
      throw InvalidDataError("Invalid value node");

    const auto read_long_value = [this, raw] {
      const auto long_value = m_reader.Read<u64>(*raw);
      if (!long_value)
        throw InvalidDataError("Invalid value node: failed to read long value");
      return *long_value;
    };

    switch (type) {
    case NodeType::String:
      return m_string_table.GetString(m_reader, *raw);
    case NodeType::Bool:
      return *raw != 0;
    case NodeType::Int:
      return S32(*raw);
    case NodeType::Float:
      return F32(util::BitCast<f32>(*raw));
    case NodeType::UInt:
      return U32(*raw);
    case NodeType::Int64:
      return S64(read_long_value());
    case NodeType::UInt64:
      return U64(read_long_value());
    case NodeType::Double:
      return F64(util::BitCast<f64>(read_long_value()));
    case NodeType::Null:
      return Byml::Null();
    default:
      throw InvalidDataError("Invalid value node: unexpected type");
    }
  }

  Byml ParseContainerChildNode(u32 offset, NodeType type) {
    if (IsContainerType(type))
      return ParseContainerNode(m_reader.Read<u32>(offset).value());
    return ParseValueNode(offset, type);
  }

  Byml ParseArrayNode(u32 offset, u32 size) {
    Byml::Array result;
    result.reserve(size);
    const u32 values_offset = offset + 4 + util::AlignUp(size, 4);
    for (u32 i = 0; i < size; ++i) {
      const auto type = m_reader.Read<NodeType>(offset + 4 + i);
      result.emplace_back(ParseContainerChildNode(values_offset + 4 * i, type.value()));
    }
    return result;
  }

  Byml ParseHashNode(u32 offset, u32 size) {
    Byml::Hash result;
    for (u32 i = 0; i < size; ++i) {
      const u32 entry_offset = offset + 4 + 8 * i;
      const auto name_idx = m_reader.ReadU24(entry_offset);
      const auto type = m_reader.Read<NodeType>(entry_offset + 3);
      result.emplace(m_hash_key_table.GetString(m_reader, name_idx.value()),
                     ParseContainerChildNode(entry_offset + 4, type.value()));
    }
    return result;
  }

  Byml ParseContainerNode(u32 offset) {
    const auto type = m_reader.Read<NodeType>(offset);
    const auto num_entries = m_reader.ReadU24();
    if (!type || !num_entries)
      throw InvalidDataError("Invalid container node");

    switch (*type) {
    case NodeType::Array:
      return ParseArrayNode(offset, *num_entries);
    case NodeType::Hash:
      return ParseHashNode(offset, *num_entries);
    default:
      throw InvalidDataError("Invalid container node: must be array or hash");
    }
  }

  util::BinaryReader m_reader;
  StringTableParser m_hash_key_table;
  StringTableParser m_string_table;
  u32 m_root_node_offset;
};

template <typename Value, typename T>
std::vector<Value> SortMapKeys(const T& map) {
  std::vector<Value> keys;
  keys.reserve(map.size());
  for (const auto& [key, value] : map)
    keys.emplace_back(key);
  std::sort(keys.begin(), keys.end());
  return keys;
}

struct WriteContext {
  WriteContext(const Byml& root, util::Endianness endianness) : writer{endianness} {
    size_t num_non_inline_nodes = 0;
    const auto traverse = [&](auto self, const Byml& data) -> void {
      const Byml::Type type = data.GetType();
      if (IsNonInlineType(type))
        ++num_non_inline_nodes;
      switch (type) {
      case Byml::Type::String:
        string_table.Add(data.GetString());
        break;
      case Byml::Type::Array:
        for (const auto& value : data.GetArray())
          self(self, value);
        break;
      case Byml::Type::Hash:
        for (const auto& [key, value] : data.GetHash()) {
          hash_key_table.Add(key);
          self(self, value);
        }
        break;
      default:
        break;
      }
    };
    traverse(traverse, root);
    non_inline_node_data.reserve(num_non_inline_nodes);
    hash_key_table.Build();
    string_table.Build();
  }

  void WriteValueNode(const Byml& data) {
    switch (data.GetType()) {
    case Byml::Type::Null:
      return writer.Write<u32>(0);
    case Byml::Type::String:
      return writer.Write<u32>(string_table.GetIndex(data.GetString()));
    case Byml::Type::Bool:
      return writer.Write<u32>(data.GetBool());
    case Byml::Type::Int:
      return writer.Write(data.GetInt());
    case Byml::Type::Float:
      return writer.Write(data.GetFloat());
    case Byml::Type::UInt:
      return writer.Write(data.GetUInt());
    case Byml::Type::Int64:
      return writer.Write(data.GetInt64());
    case Byml::Type::UInt64:
      return writer.Write(data.GetUInt64());
    case Byml::Type::Double:
      return writer.Write(data.GetDouble());
    default:
      throw std::logic_error("Unexpected value node type");
    }
  }

  struct NonInlineNode {
    size_t offset_in_container;
    const Byml* data;
  };

  void WriteContainerNode(const Byml& data) {
    std::vector<NonInlineNode> non_inline_nodes;

    const auto write_container_item = [&](const Byml& item) {
      if (IsNonInlineType(item.GetType())) {
        non_inline_nodes.push_back({writer.Tell(), &item});
        writer.Write<u32>(0);
      } else {
        WriteValueNode(item);
      }
    };

    switch (data.GetType()) {
    case Byml::Type::Array: {
      const auto& array = data.GetArray();
      writer.Write(NodeType::Array);
      writer.WriteU24(array.size());
      for (const auto& item : array)
        writer.Write(GetNodeType(item.GetType()));
      writer.AlignUp(4);
      for (const auto& item : array)
        write_container_item(item);
      break;
    }
    case Byml::Type::Hash: {
      const auto& hash = data.GetHash();
      writer.Write(NodeType::Hash);
      writer.WriteU24(hash.size());
      for (const auto& [key, value] : hash) {
        const auto type = GetNodeType(value.GetType());
        writer.WriteU24(hash_key_table.GetIndex(key));
        writer.Write(type);
        write_container_item(value);
      }
      break;
    }
    default:
      throw std::invalid_argument("Invalid container node type");
    }

    for (const NonInlineNode& node : non_inline_nodes) {
      const auto it = non_inline_node_data.find(*node.data);
      if (it != non_inline_node_data.end()) {
        // This node has already been written. Reuse its data.
        writer.RunAt(node.offset_in_container, [&](size_t) { writer.Write<u32>(it->second); });
      } else {
        const size_t offset = writer.Tell();
        writer.RunAt(node.offset_in_container, [&](size_t) { writer.Write<u32>(offset); });
        non_inline_node_data.emplace(*node.data, offset);
        if (IsLongType(node.data->GetType()))
          WriteValueNode(*node.data);
        else
          WriteContainerNode(*node.data);
      }
    }
  }

  struct StringTable {
    explicit operator bool() const { return !sorted_strings.empty(); }
    size_t Size() const { return sorted_strings.size(); }
    void Add(std::string_view string) { map.emplace(string, 0); }
    u32 GetIndex(std::string_view string) const { return map.at(string); }

    /// Build the sorted vector of strings and sets indices in the map.
    void Build() {
      sorted_strings = SortMapKeys<std::string_view>(map);
      for (const auto& [i, key] : util::Enumerate(sorted_strings))
        map[key] = i;
    }

    // We use a hash map here to get fast insertions and fast lookups in hot paths,
    // and because we only need a sorted list of strings for two operations.
    absl::flat_hash_map<std::string_view, u32> map;
    std::vector<std::string_view> sorted_strings;
  };

  void WriteStringTable(const StringTable& table) {
    const size_t base = writer.Tell();
    writer.Write(NodeType::StringTable);
    writer.WriteU24(table.Size());

    // String offsets.
    const size_t offset_table_offset = writer.Tell();
    writer.Seek(writer.Tell() + sizeof(u32) * (table.Size() + 1));

    for (const auto& [i, string] : util::Enumerate(table.sorted_strings)) {
      writer.WriteCurrentOffsetAt<u32>(offset_table_offset + sizeof(u32) * i, base);
      writer.WriteCStr(string);
    }

    writer.WriteCurrentOffsetAt<u32>(offset_table_offset + sizeof(u32) * table.Size(), base);
    writer.AlignUp(4);
  }

  util::BinaryWriter writer;
  StringTable hash_key_table;
  StringTable string_table;
  absl::flat_hash_map<std::reference_wrapper<const Byml>, u32> non_inline_node_data;
};

}  // namespace byml

Byml Byml::FromBinary(tcb::span<const u8> data) {
  byml::Parser parser{data};
  return parser.Parse();
}

std::vector<u8> Byml::ToBinary(bool big_endian, int version) const {
  if (!byml::IsValidVersion(version))
    throw std::invalid_argument("Invalid version");

  byml::WriteContext ctx{*this, big_endian ? util::Endianness::Big : util::Endianness::Little};

  // Header
  ctx.writer.Write(ctx.writer.Endian() == util::Endianness::Big ? "BY" : "YB");
  ctx.writer.Write<u16>(version);
  ctx.writer.Write<u32>(0);  // Hash key table offset.
  ctx.writer.Write<u32>(0);  // String table offset.
  ctx.writer.Write<u32>(0);  // Root node offset.

  if (GetType() == Byml::Type::Null)
    return ctx.writer.Finalize();

  if (ctx.hash_key_table) {
    ctx.writer.WriteCurrentOffsetAt<u32>(offsetof(byml::ResHeader, hash_key_table_offset));
    ctx.WriteStringTable(ctx.hash_key_table);
  }

  if (ctx.string_table) {
    ctx.writer.WriteCurrentOffsetAt<u32>(offsetof(byml::ResHeader, string_table_offset));
    ctx.WriteStringTable(ctx.string_table);
  }

  ctx.writer.WriteCurrentOffsetAt<u32>(offsetof(byml::ResHeader, root_node_offset));
  ctx.writer.AlignUp(4);
  ctx.WriteContainerNode(*this);
  ctx.writer.AlignUp(4);
  return ctx.writer.Finalize();
}

const Byml::Hash& Byml::GetHash() const {
  return Get<Type::Hash>();
}

const Byml::Array& Byml::GetArray() const {
  return Get<Type::Array>();
}

const Byml::String& Byml::GetString() const {
  return Get<Type::String>();
}

bool Byml::GetBool() const {
  return Get<Type::Bool>();
}

s32 Byml::GetInt() const {
  switch (GetType()) {
  case Type::Int:
    return Get<Type::Int>();
  case Type::UInt:
    return s32(Get<Type::UInt>());
  default:
    throw TypeError("GetInt: expected Int or UInt");
  }
}

template <typename T>
static inline T CheckPositiveAndReturn(T value) {
  if (value >= 0)
    return value;
  throw TypeError("expected positive integer value");
}

u32 Byml::GetUInt() const {
  switch (GetType()) {
  case Type::UInt:
    return Get<Type::UInt>();
  case Type::Int:
    return CheckPositiveAndReturn(Get<Type::Int>());
  default:
    throw TypeError("GetUInt: expected Int or UInt value");
  }
}

f32 Byml::GetFloat() const {
  return Get<Type::Float>();
}

s64 Byml::GetInt64() const {
  switch (GetType()) {
  case Type::Int:
    return Get<Type::Int>();
  case Type::UInt:
    return s64(Get<Type::UInt>());
  case Type::Int64:
    return Get<Type::Int64>();
  default:
    throw TypeError("GetInt64: expected Int, UInt or Int64");
  }
}

u64 Byml::GetUInt64() const {
  switch (GetType()) {
  case Type::Int:
    return CheckPositiveAndReturn(Get<Type::Int>());
  case Type::UInt:
    return Get<Type::UInt>();
  case Type::UInt64:
    return Get<Type::UInt64>();
  case Type::Int64:
    return CheckPositiveAndReturn(Get<Type::Int64>());
  default:
    throw TypeError("GetUInt64: expected UInt or UInt64");
  }
}

f64 Byml::GetDouble() const {
  return Get<Type::Double>();
}

}  // namespace oead
