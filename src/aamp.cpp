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

#include <oead/aamp.h>

#include <absl/container/inlined_vector.h>
#include <absl/strings/str_format.h>
#include <algorithm>
#include <array>
#include <limits>
#include <queue>
#include "absl/container/flat_hash_set.h"

#include <oead/errors.h>
#include "util/binary_reader.h"
#include "util/bit_utils.h"
#include "util/iterator_utils.h"
#include "util/type_utils.h"

namespace oead::aamp {

constexpr std::array<char, 4> HeaderMagic = {'A', 'A', 'M', 'P'};

enum class HeaderFlag : u32 {
  LittleEndian = 1 << 0,
  Utf8 = 1 << 1,
};

struct ResHeader {
  std::array<char, 4> magic;
  util::LeInt<u32> version;
  util::Flags<HeaderFlag> flags;
  util::LeInt<u32> file_size;
  util::LeInt<u32> pio_version;
  /// Offset to parameter IO (relative to 0x30)
  util::LeInt<u32> offset_to_pio;
  /// Number of lists (including parameter IO)
  util::LeInt<u32> num_lists;
  util::LeInt<u32> num_objects;
  util::LeInt<u32> num_parameters;
  util::LeInt<u32> data_section_size;
  util::LeInt<u32> string_section_size;
  util::LeInt<u32> unk_section_size;
};
static_assert(sizeof(ResHeader) == 0x30);

template <typename T, size_t Factor = 4>
struct CompactOffset {
  static constexpr size_t MaxDistance = [] {
    if constexpr (util::IsAnyOfType<T, U24<false>, U24<true>>())
      return Factor * (1 << 24);
    else
      return Factor * std::numeric_limits<typename NumberType<T>::type>::max();
  }();

  constexpr CompactOffset() = default;
  constexpr CompactOffset(size_t value) { Set(value); }
  constexpr size_t Get() const { return size_t(raw_value) * Factor; }
  constexpr void Set(size_t x) {
    if (x % Factor != 0 || x > MaxDistance)
      throw std::invalid_argument("Offset is not representable");
    raw_value = x / Factor;
  }

private:
  T raw_value;
};

struct ResParameter {
  util::LeInt<u32> name_crc32;
  CompactOffset<U24<false>> data_rel_offset;
  Parameter::Type type;
};
static_assert(sizeof(ResParameter) == 8);

struct ResParameterObj {
  util::LeInt<u32> name_crc32;
  CompactOffset<util::LeInt<u16>> parameters_rel_offset;
  util::LeInt<u16> num_parameters;
};
static_assert(sizeof(ResParameterObj) == 8);

struct ResParameterList {
  util::LeInt<u32> name_crc32;
  CompactOffset<util::LeInt<u16>> lists_rel_offset;
  util::LeInt<u16> num_lists;
  CompactOffset<util::LeInt<u16>> objects_rel_offset;
  util::LeInt<u16> num_objects;
};
static_assert(sizeof(ResParameterList) == 0xc);

class Parser {
public:
  Parser(tcb::span<const u8> data) : m_reader{data, util::Endianness::Little} {
    if (data.size() < sizeof(ResHeader))
      throw InvalidDataError("Invalid header");

    if (m_reader.Read<decltype(ResHeader::magic)>() != HeaderMagic)
      throw InvalidDataError("Invalid magic");

    const auto version = *m_reader.Read<u32>(offsetof(ResHeader, version));
    if (version != 2)
      throw InvalidDataError("Only version 2 parameter archives are supported");

    auto flags = *m_reader.Read<util::Flags<HeaderFlag>>(offsetof(ResHeader, flags));
    if (!flags[HeaderFlag::LittleEndian])
      throw InvalidDataError("Only little endian parameter archives are supported");
    if (!flags[HeaderFlag::Utf8])
      throw InvalidDataError("Only UTF-8 parameter archives are supported");
  }

  ParameterIO Parse() {
    const auto offset_to_pio = *m_reader.Read<u32>(offsetof(ResHeader, offset_to_pio));
    auto&& [root_name, root] = ParseList(sizeof(ResHeader) + offset_to_pio);
    if (root_name != ParameterIO::ParamRootKey.hash)
      throw InvalidDataError("No param_root");
    ParameterIO pio;
    pio.version = *m_reader.Read<u32>(offsetof(ResHeader, pio_version));
    pio.type = m_reader.ReadString(sizeof(ResHeader));
    pio.objects = std::move(root.objects);
    pio.lists = std::move(root.lists);
    return pio;
  }

private:
  std::pair<u32, Parameter> ParseParameter(u32 offset) {
    const auto info = m_reader.Read<ResParameter>(offset).value();
    const auto crc32 = info.name_crc32;
    const auto data_offset = offset + info.data_rel_offset.Get();

    switch (info.type) {
    case Parameter::Type::Bool:
      return {crc32, m_reader.Read<u32>(data_offset).value() != 0};
    case Parameter::Type::F32:
      // There's some trickery going on in the parse function -- floats can
      // in some cases get multiplied by some factor.
      // That is currently ignored and the data is loaded as is.
      return {crc32, m_reader.Read<f32>(data_offset).value()};
    case Parameter::Type::Int:
      return {crc32, m_reader.Read<int>(data_offset).value()};
    case Parameter::Type::Vec2:
      return {crc32, m_reader.Read<Vector2f>(data_offset).value()};
    case Parameter::Type::Vec3:
      return {crc32, m_reader.Read<Vector3f>(data_offset).value()};
    case Parameter::Type::Vec4:
      return {crc32, m_reader.Read<Vector4f>(data_offset).value()};
    case Parameter::Type::Color:
      return {crc32, m_reader.Read<Color4f>(data_offset).value()};
    case Parameter::Type::String32:
      return {crc32, FixedSafeString<32>(m_reader.ReadString(data_offset, 32))};
    case Parameter::Type::String64:
      return {crc32, FixedSafeString<64>(m_reader.ReadString(data_offset, 64))};
    case Parameter::Type::Curve1:
      return {crc32, m_reader.Read<std::array<Curve, 1>>(data_offset).value()};
    case Parameter::Type::Curve2:
      return {crc32, m_reader.Read<std::array<Curve, 2>>(data_offset).value()};
    case Parameter::Type::Curve3:
      return {crc32, m_reader.Read<std::array<Curve, 3>>(data_offset).value()};
    case Parameter::Type::Curve4:
      return {crc32, m_reader.Read<std::array<Curve, 4>>(data_offset).value()};
    case Parameter::Type::BufferInt:
      return {crc32, ParseBuffer<int>(data_offset)};
    case Parameter::Type::BufferF32:
      return {crc32, ParseBuffer<f32>(data_offset)};
    case Parameter::Type::String256:
      return {crc32, FixedSafeString<256>(m_reader.ReadString(data_offset, 256))};
    case Parameter::Type::Quat:
      // Quat parameters receive additional processing after being loaded:
      // depending on what parameters are passed to the apply function,
      // there may be linear interpolation going on.
      // That is also being ignored by this implementation.
      return {crc32, m_reader.Read<Quatf>(data_offset).value()};
    case Parameter::Type::U32:
      return {crc32, m_reader.Read<U32>(data_offset).value()};
    case Parameter::Type::BufferU32:
      return {crc32, ParseBuffer<u32>(data_offset)};
    case Parameter::Type::BufferBinary:
      return {crc32, ParseBuffer<u8>(data_offset)};
    case Parameter::Type::StringRef:
      return {crc32, m_reader.ReadString(data_offset)};
    default:
      throw InvalidDataError("Unexpected parameter type");
    }
  }

  template <typename T>
  std::vector<T> ParseBuffer(u32 data_offset) {
    const size_t size = m_reader.Read<u32>(data_offset - 4).value();
    std::vector<T> buffer;
    buffer.reserve(size);
    for (size_t i = 0; i < size; ++i)
      buffer.emplace_back(m_reader.Read<T>().value());
    return buffer;
  }

  std::pair<u32, ParameterObject> ParseObject(u32 offset) {
    const auto info = m_reader.Read<ResParameterObj>(offset).value();
    const auto offset_to_params = offset + info.parameters_rel_offset.Get();

    ParameterObject object;
    object.params.reserve(info.num_parameters);
    for (size_t i = 0; i < info.num_parameters; ++i)
      object.params.emplace(ParseParameter(offset_to_params + sizeof(ResParameter) * i));
    return {info.name_crc32, std::move(object)};
  }

  std::pair<u32, ParameterList> ParseList(u32 offset) {
    const auto info = m_reader.Read<ResParameterList>(offset).value();
    const auto offset_to_lists = offset + info.lists_rel_offset.Get();
    const auto offset_to_objects = offset + info.objects_rel_offset.Get();

    ParameterList list;
    list.lists.reserve(info.num_lists);
    list.objects.reserve(info.num_objects);
    for (size_t i = 0; i < info.num_lists; ++i)
      list.lists.emplace(ParseList(offset_to_lists + sizeof(ResParameterList) * i));
    for (size_t i = 0; i < info.num_objects; ++i)
      list.objects.emplace(ParseObject(offset_to_objects + sizeof(ResParameterObj) * i));
    return {info.name_crc32, std::move(list)};
  }

  util::BinaryReader m_reader;
};

template <typename T, typename T2>
static void WriteBuffer(util::BinaryWriterBase<T2>& writer, const std::vector<T>& v) {
  writer.Write(u32(v.size()));
  for (const auto& x : v)
    writer.Write(x);
}

struct WriteContext {
public:
  void WriteLists(const ParameterIO& pio) {
    const auto write = [&](auto self, const ParameterList& list) -> void {
      WriteOffsetForParent(list, offsetof(ResParameterList, lists_rel_offset));
      for (const auto& pair : list.lists)
        WriteList(pair.first, pair.second);
      for (const auto& pair : list.lists)
        self(self, pair.second);
    };
    WriteList(ParameterIO::ParamRootKey, pio);
    write(write, pio);
  }

  void WriteObjects(const ParameterList& list) {
    // Perform a DFS on the parameter tree. Objects are handled before lists.
    WriteOffsetForParent(list, offsetof(ResParameterList, objects_rel_offset));
    for (const auto& [object_name, object] : list.objects)
      WriteObject(object_name, object);

    for (const auto& [child_list_name, child_list] : list.lists)
      WriteObjects(child_list);
  }

  void WriteParameters(const ParameterList& list) {
    // Perform a DFS on the parameter tree. Objects are handled after lists.
    for (const auto& [child_list_name, child_list] : list.lists)
      WriteParameters(child_list);

    for (const auto& [object_name, object] : list.objects) {
      WriteOffsetForParent(object, offsetof(ResParameterObj, parameters_rel_offset));
      for (const auto& [name, param] : object.params) {
        WriteParameter(name, param);
      }
    }
  }

  void CollectParameters(const ParameterIO& pio) {
    // For some reason, the order in which parameter data is serialized is not the order
    // of parameter objects or even parameters... Rather, for the majority of binary
    // parameter archives the order is determined with a rather convoluted algorithm:
    //
    // * First, process all of the parameter IO's objects (i.e. add all their parameters
    //   to the parameter queue).
    // * Recursively collect all objects for child lists. For lists, object processing
    //   happens after recursively processing child lists; however every 2 lists one
    //   object from the parent list is processed.
    //
    const auto do_collect = [this](auto next, const ParameterList& list,
                                   bool process_top_objects_first) -> void {
      auto object_it = list.objects.begin();
      const auto process_one_object = [this, &object_it] {
        for (const auto& [name, param] : object_it->second.params) {
          if (IsStringType(param.GetType()))
            string_parameters_to_write.emplace_back(param);
          else
            parameters_to_write.emplace_back(param);
        }
        ++object_it;
      };

      // If the parameter IO is a Breath of the Wild AIProgram, then it appears that
      // even the parameter IO's objects are processed after child lists.
      // This is likely a hack, but it does match observations...
      const bool is_botw_aiprog =
          !list.objects.empty() && list.objects.begin()->first == Name("DemoAIActionIdx");

      if (process_top_objects_first && !is_botw_aiprog) {
        // Again this is probably a hack but it is required for matching BoneControl documents...
        for (size_t i = 0; i < 7 && object_it != list.objects.end(); ++i)
          process_one_object();
      }

      size_t i = 0;
      for (const auto& [child_list_name, child_list] : list.lists) {
        if (!is_botw_aiprog && i % 2 == 0 && object_it != list.objects.end())
          process_one_object();
        next(next, child_list, false);
        ++i;
      }

      // Process all remaining objects.
      while (object_it != list.objects.end())
        process_one_object();
    };
    do_collect(do_collect, pio, true);
  }

  void WriteDataSection() {
    const size_t lookup_start_offset = writer.Tell();
    for (const Parameter& param : parameters_to_write)
      WriteParameterData(param, lookup_start_offset);
    writer.AlignUp(4);
  }

  void WriteStringSection() {
    for (const Parameter& param : string_parameters_to_write)
      WriteString(param);
    writer.AlignUp(4);
  }

  void WriteParameterData(const Parameter& param, size_t lookup_start_offset) {
    if (IsStringType(param.GetType()))
      throw std::logic_error("WriteParameterData called with string parameter");

    // Write to a temporary buffer first to try to reuse existing data.
    util::BinaryWriterBase<absl::InlinedVector<u8, 0x200>> temp_writer{writer.Endian()};
    util::Match(
        param.GetVariant().v,  //
        [&](bool v) { temp_writer.Write<u32>(v); },
        [&](const std::vector<int>& v) { WriteBuffer(temp_writer, v); },
        [&](const std::vector<f32>& v) { WriteBuffer(temp_writer, v); },
        [&](const std::vector<u32>& v) { WriteBuffer(temp_writer, v); },
        [&](const std::vector<u8>& v) { WriteBuffer(temp_writer, v); },
        [&](const auto& v) { temp_writer.Write(v); });

    const size_t parent_offset = offsets.at(&param);
    size_t data_offset = writer.Tell() + (IsBufferType(param.GetType()) ? 4 : 0);
    bool found = false;

    for (size_t offset = lookup_start_offset;
         offset + temp_writer.Buffer().size() <= writer.Buffer().size() &&
         offset - parent_offset < (1 << 24) * 4;
         offset += 4) {
      if (std::equal(temp_writer.Buffer().begin(), temp_writer.Buffer().end(),
                     writer.Buffer().begin() + offset)) {
        data_offset = offset;
        found = true;
        break;
      }
    }

    // Write the data offset in the parent parameter structure.
    writer.RunAt(parent_offset + offsetof(ResParameter, data_rel_offset), [&](size_t) {
      writer.Write(
          static_cast<decltype(ResParameter::data_rel_offset)>(data_offset - parent_offset));
    });

    // Write the parameter data if it hasn't already been written.
    if (!found) {
      writer.WriteBytes(temp_writer.Buffer());
      writer.AlignUp(4);
    }
  }

  void WriteString(const Parameter& param) {
    const size_t parent_offset = offsets.at(&param);
    const std::string_view string = param.GetStringView();
    const auto pair = string_offsets.emplace(string, u32(writer.Tell()));

    // Write the data offset in the parent parameter structure.
    writer.RunAt(parent_offset + offsetof(ResParameter, data_rel_offset), [&](size_t) {
      writer.Write(
          static_cast<decltype(ResParameter::data_rel_offset)>(pair.first->second - parent_offset));
    });

    // Write the parameter data if it hasn't already been written.
    if (pair.second) {
      writer.WriteCStr(string);
      writer.AlignUp(4);
    }
  }

  void WriteList(Name name, const ParameterList& list) {
    offsets.emplace(&list, u32(writer.Tell()));
    ++num_lists;
    ResParameterList data;
    data.name_crc32 = name.hash;
    data.num_lists = u16(list.lists.size());
    data.num_objects = u16(list.objects.size());
    writer.Write(data);
  }

  void WriteObject(Name name, const ParameterObject& object) {
    offsets.emplace(&object, u32(writer.Tell()));
    ++num_objects;
    ResParameterObj data;
    data.name_crc32 = name.hash;
    data.num_parameters = u16(object.params.size());
    writer.Write(data);
  }

  void WriteParameter(Name name, const Parameter& parameter) {
    offsets.emplace(&parameter, u32(writer.Tell()));
    ++num_parameters;
    ResParameter data;
    data.name_crc32 = name.hash;
    data.type = parameter.GetType();
    writer.Write(data);
  }

  template <typename T>
  void WriteOffsetForParent(const T& parent, size_t offset_in_parent_struct) {
    const u32 parent_offset = offsets.at(&parent);
    writer.WriteCurrentOffsetAt<CompactOffset<util::LeInt<u16>>>(
        parent_offset + offset_in_parent_struct, parent_offset);
  }

  util::BinaryWriter writer{util::Endianness::Little};
  u32 num_lists = 0;
  u32 num_objects = 0;
  u32 num_parameters = 0;
  /// Parameters in serialization order.
  std::vector<std::reference_wrapper<const Parameter>> parameters_to_write;
  std::vector<std::reference_wrapper<const Parameter>> string_parameters_to_write;
  /// Used to find where a structure (ResParameter...) is located in the buffer.
  absl::flat_hash_map<const void*, u32> offsets;
  absl::flat_hash_map<std::string_view, u32> string_offsets;
};

ParameterIO ParameterIO::FromBinary(tcb::span<const u8> data) {
  Parser parser{data};
  return parser.Parse();
}

std::vector<u8> ParameterIO::ToBinary() const {
  WriteContext ctx;
  ctx.writer.Seek(sizeof(ResHeader));
  ctx.writer.WriteCStr(type);
  ctx.writer.AlignUp(4);
  const size_t offset_to_pio = ctx.writer.Tell();

  ctx.WriteLists(*this);
  ctx.WriteObjects(*this);
  ctx.CollectParameters(*this);
  ctx.WriteParameters(*this);

  const size_t data_section_begin = ctx.writer.Tell();
  ctx.WriteDataSection();

  const size_t string_section_begin = ctx.writer.Tell();
  ctx.WriteStringSection();

  const size_t unk_section_begin = ctx.writer.Tell();
  ctx.writer.AlignUp(4);
  ctx.writer.GrowBuffer();

  ResHeader header{};
  header.magic = HeaderMagic;
  header.version = 2;
  header.flags[HeaderFlag::LittleEndian] = true;
  header.flags[HeaderFlag::Utf8] = true;
  header.file_size = ctx.writer.Tell();
  header.pio_version = version;
  header.offset_to_pio = u32(offset_to_pio - sizeof(ResHeader));
  header.num_lists = ctx.num_lists;
  header.num_objects = ctx.num_objects;
  header.num_parameters = ctx.num_parameters;
  header.data_section_size = string_section_begin - data_section_begin;
  header.string_section_size = unk_section_begin - string_section_begin;
  header.unk_section_size = 0;
  ctx.writer.Seek(0);
  ctx.writer.Write(header);
  return ctx.writer.Finalize();
}

std::string_view Parameter::GetStringView() const {
  if (!IsStringType(GetType()))
    throw TypeError("GetStringView called with non-string parameter");
  return util::Match(GetVariant().v, [](const auto& v) -> std::string_view {
    if constexpr (std::is_convertible<decltype(v), std::string_view>())
      return Str(v);
    else
      std::terminate();
  });
}

}  // namespace oead::aamp
