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

#include <absl/strings/str_format.h>
#include <algorithm>
#include <any>

#include <oead/errors.h>
#include <oead/gsheet.h>
#include <oead/util/align.h>
#include <oead/util/magic_utils.h>

namespace oead::gsheet {

FieldMap MakeFieldMap(tcb::span<ResField> fields) {
  FieldMap map;
  for (ResField& field : fields)
    map.emplace(field.name, &field);
  return map;
}

namespace {
struct OpaqueArray {
  auto Items(size_t item_size) const {
    return easy_iterator::MakeIterable<OpaqueIterator>(
        data, reinterpret_cast<void*>((uintptr_t)data + item_size * size), item_size);
  }

  void* data = nullptr;
  u32 size = 0;

private:
  [[maybe_unused]] u32 padding = 0;
};
static_assert(sizeof(OpaqueArray) == 0x10);

u32 GetValueSize(const SheetRw& sheet) {
  if (sheet.root_fields.empty())
    return 0;
  const Field& last_field = sheet.root_fields.back();
  return util::AlignUp<u32>(last_field.offset_in_value + last_field.inline_size, sheet.alignment);
}

u32 GetNumFields(const SheetRw& sheet) {
  u32 count = 0;
  const auto traverse = [&count](auto self, const Field& field) -> void {
    ++count;
    for (const auto& subfield : field.fields)
      self(self, subfield);
  };
  for (const auto& field : sheet.root_fields)
    traverse(traverse, field);
  return count;
}

struct Writer {
  std::vector<u8> Write(const SheetRw& sheet) {
    // Header
    ResHeader header{};
    header.alignment = sheet.alignment;
    header.hash = sheet.hash;
    header.num_root_fields = u32(sheet.root_fields.size());
    header.num_fields = GetNumFields(sheet);
    header.num_values = u32(sheet.values.size());
    header.value_size = GetValueSize(sheet);
    writer.Write(header);
    field_strings.push_back({offsetof(ResHeader, name), sheet.name});
    writer.RunAt(offsetof(ResHeader, values),
                 [&](auto) { RegisterAndWriteObjectPtr(sheet.values, "sheet.values"); });

    // Fields
    WriteFields(sheet.root_fields, true);

    // Values
    RegisterObject(sheet.values);
    for (const auto& value : sheet.values) {
      WriteStruct(value, sheet.root_fields);
      writer.AlignUp(sheet.alignment);
    }

    // Document name and field strings
    writer.AlignUp(0x10);
    for (const auto& [ptr_offset, string] : field_strings) {
      writer.WriteCurrentOffsetAt<u64>(ptr_offset);
      writer.WriteCStr(string);
    }

    // Value data
    for (const auto& value : sheet.values)
      WriteStructData(value, sheet.root_fields);

    writer.AlignUp(0x10);
    writer.GrowBuffer();

    // Pointers
    WriteObjectPointers();

    return writer.Finalize();
  }

private:
  void WriteField(const Field& field) {
    const auto offset = writer.Tell();
    ResField res{};
    res.type = field.type;
    res.x11 = field.x11;
    res.flags = field.flags;
    res.offset_in_value = field.offset_in_value;
    res.inline_size = field.inline_size;
    res.data_size = field.data_size;
    res.num_fields = u16(field.fields.size());
    res.parent = (ResField*)0xdeadbeefdeadbeef;
    writer.Write(res);
    field_strings.push_back({offset + offsetof(ResField, name), field.name});
    field_strings.push_back({offset + offsetof(ResField, type_name), field.type_name});
    if (!field.fields.empty()) {
      writer.RunAt(offset + offsetof(ResField, fields),
                   [&](auto) { RegisterAndWriteObjectPtr(field.fields, "field.fields"); });
    }
  }

  void WriteFields(const std::vector<Field>& fields, bool is_root = false) {
    if (!is_root && !fields.empty())
      RegisterObject(fields);
    for (const auto& field : fields)
      WriteField(field);
    for (const auto& field : fields)
      WriteFields(field.fields);
  }

  void WriteDataInline(const Data& data, const Field& field) {
    util::Match(
        data.v.v,
        [&](const Data::Struct& v) {
          if (field.type != Field::Type::Struct)
            throw std::invalid_argument("Mismatched field type and data type (expected Struct)");
          WriteStruct(v, field.fields);
        },
        [&](bool v) {
          if (field.type != Field::Type::Bool)
            throw std::invalid_argument("Mismatched field type and data type (expected Bool)");
          writer.Write<u8>(v);
        },
        [&](int v) {
          if (field.type != Field::Type::Int)
            throw std::invalid_argument("Mismatched field type and data type (expected Int)");
          writer.Write(v);
        },
        [&](float v) {
          if (field.type != Field::Type::Float)
            throw std::invalid_argument("Mismatched field type and data type (expected Float)");
          writer.Write(v);
        },
        [&](const std::string& v) {
          if (field.type != Field::Type::String)
            throw std::invalid_argument("Mismatched field type and data type (expected String)");
          writer.WriteCStr(v);
        },
        [](const auto&) { throw std::logic_error("Unexpected type"); });
  }

  void WriteStringPtr(const std::string& string, const Field& field) {
    if (!string.empty() || !field.flags[Field::Flag::IsNullable])
      RegisterAndWriteObjectPtr(string, "string ptr");
    else
      writer.Write<u64>(0);
    writer.Write(u32(string.size()));
    writer.Write<u32>(0);
  }

  void WriteStruct(const Data::Struct& struct_, const std::vector<Field>& fields) {
    const auto base_offset = writer.Tell();
    for (const Field& field : fields) {
      writer.Seek(base_offset + field.offset_in_value);
      const auto& data = struct_.at(field.name);
      // Arrays.
      if (field.flags[Field::Flag::IsArray]) {
        RegisterAndWriteObjectPtr(data, "struct: array");
        writer.Write(u32(data.VisitArray([](const auto& v) { return v.size(); })));
        writer.Write<u32>(0);
      }
      // Strings (including Nullables that are strings).
      else if (field.type == Field::Type::String) {
        WriteStringPtr(data.v.Get<Data::Type::String>(), field);
      }
      // Nullables that are not strings.
      else if (field.flags[Field::Flag::IsNullable]) {
        if (data.IsNull())
          writer.Write<u64>(0);
        else
          RegisterAndWriteObjectPtr(data, "struct: nullable");
      }
      // Other types.
      else {
        WriteDataInline(data, field);
      }
    }
  }

  void WriteStructData(const Data::Struct& struct_, const std::vector<Field>& fields) {
    for (const Field& field : fields) {
      const auto& data = struct_.at(field.name);
      // Arrays.
      if (field.flags[Field::Flag::IsArray]) {
        switch (field.type) {
        case Field::Type::Struct: {
          const auto& structs = data.v.Get<Data::Type::StructArray>();
          for (const auto& struct_ : structs)
            WriteStructData(struct_, field.fields);

          writer.AlignUp(8);
          RegisterObject(data);
          for (const auto& struct_ : structs) {
            WriteStruct(struct_, field.fields);
            writer.AlignUp(8);
          }
          break;
        }
        case Field::Type::String: {
          const auto& strings = data.v.Get<Data::Type::StringArray>();
          for (const auto& string : strings) {
            RegisterObject(string);
            writer.WriteCStr(string);
          }

          writer.AlignUp(8);
          RegisterObject(data);
          for (const auto& string : strings)
            WriteStringPtr(string, field);
          break;
        }
        default: {
          // XXX: how are bool arrays written?
          if (field.type != Field::Type::Bool)
            writer.AlignUp(4);

          RegisterObject(data);
          data.VisitArray([&](const auto& v) {
            using T = typename std::decay_t<decltype(v)>::value_type;
            if constexpr (util::IsAnyOfType<T, bool, int, float>()) {
              for (const T item : v) {
                writer.Write(item);
                writer.AlignUp(4);
              }
            }
          });
        }
        }
      }
      // Strings.
      else if (field.type == Field::Type::String) {
        const auto& string = data.v.Get<Data::Type::String>();
        if (!string.empty() || !field.flags[Field::Flag::IsNullable]) {
          RegisterObject(string);
          writer.WriteCStr(string);
        }
      }
      // Nullables.
      else if (field.flags[Field::Flag::IsNullable]) {
        if (!data.IsNull()) {
          if (field.type == Field::Type::Struct) {
            const auto& struct_ = data.v.Get<Data::Type::Struct>();
            WriteStructData(struct_, field.fields);
            writer.AlignUp(8);
            RegisterObject(data);
            WriteStruct(struct_, field.fields);
          } else {
            RegisterObject(data);
            WriteDataInline(data, field);
          }
        }
      }
      // Structs.
      else if (field.type == Field::Type::Struct) {
        WriteStructData(data.v.Get<Data::Type::Struct>(), field.fields);
      }
      // Nothing to do for the other inline types (bool/int/float).
    }
  }

  template <typename T>
  void RegisterObject(const T& obj) {
    static_assert(!std::is_pointer_v<std::decay_t<T>>);
    ObjectEntry& entry = objects[&obj];
    if (entry.obj_offset)
      throw std::logic_error("Attempted to register an object twice");
    entry.obj_offset = writer.Tell();
  }

  template <typename T>
  void RegisterAndWriteObjectPtr(const T& obj, [[maybe_unused]] const char* description) {
    static_assert(!std::is_pointer_v<std::decay_t<T>>);
    ObjectEntry& entry = objects[&obj];
    if (entry.ptr_offset)
      throw std::logic_error("Attempted to register a pointer twice");
    entry.ptr_offset = writer.Tell();
#ifdef OEAD_GSHEET_DEBUG
    entry.object = obj;
    entry.description = description;
#endif
    writer.Write<u64>(0xFFFFFFFFFFFFFFFF);
  }

  void WriteObjectPointers() {
    for (const auto& [obj, entry] : objects) {
      if (!entry.ptr_offset && !entry.obj_offset)
        throw std::logic_error("Invalid object entry");

      if (!entry.ptr_offset) {
        throw std::logic_error(absl::StrFormat(
            "Inaccessible object: no pointer was written for object %#x", entry.obj_offset));
      }

      if (!entry.obj_offset) {
        throw std::logic_error(absl::StrFormat(
            "Missing object: no object was written for pointer %#x", entry.ptr_offset));
      }

      writer.RunAt(entry.ptr_offset,
                   [this, offset = entry.obj_offset](auto) { writer.Write<u64>(offset); });
    }
  }

  util::BinaryWriter writer{util::Endianness::Little};

  /// List of field-related strings and their corresponding offsets.
  std::vector<std::pair<u32, std::string_view>> field_strings;

  struct ObjectEntry {
    /// Offset of the pointer that is to point to the object.
    u32 ptr_offset = 0;
    /// Offset of the object itself.
    u32 obj_offset = 0;
#ifdef OEAD_GSHEET_DEBUG
    std::any object;
    const char* description = nullptr;
#endif
  };
  absl::flat_hash_map<const void*, ObjectEntry> objects;
};
}  // namespace

std::vector<u8> SheetRw::ToBinary() const {
  return Writer{}.Write(*this);
}

namespace {
void RelocateField(ResField& field, ResField* parent, tcb::span<u8> buffer) {
  if (!field.name || !field.type_name)
    throw InvalidDataError("Missing field name or field type name");

  util::Relocate(buffer, field.name);
  util::Relocate(buffer, field.type_name);

  const auto fields_offset = reinterpret_cast<uintptr_t>(field.fields);
  if (fields_offset) {
    if (fields_offset < sizeof(ResHeader))
      throw InvalidDataError("Invalid field offset");
    if (fields_offset % sizeof(ResField) != 0)
      throw InvalidDataError("Invalid field alignment");

    util::Relocate(buffer, field.fields, field.num_fields);
    field.parent = parent;
    for (size_t i = 0; i < field.num_fields; ++i) {
      RelocateField(field.fields[i], &field, buffer);
    }
  } else if (field.num_fields) {
    throw InvalidDataError("Missing sub-fields");
  }
}

void RelocateFieldData(void* data, const ResField& field, tcb::span<u8> buffer,
                       bool ignore_array_flag = false, bool ignore_nullable_flag = false) {
  if (field.flags[Field::Flag::IsArray] && !ignore_array_flag) {
    auto* array = static_cast<OpaqueArray*>(data);

    util::RelocateWithSize(buffer, array->data, field.data_size * array->size);
    for (void* item : array->Items(field.data_size))
      RelocateFieldData(item, field, buffer, true, ignore_nullable_flag);
  }

  else if (field.type == Field::Type::String) {
    auto* string = static_cast<String*>(data);

    if ((string->size || !field.flags[Field::Flag::IsNullable]) && !string->data)
      throw InvalidDataError("Missing string data");

    if (string->data) {
      util::Relocate(buffer, string->data, string->size);
      if (util::ReadString(buffer, string->data).size() != string->size)
        throw InvalidDataError("Invalid string size");
    }
  }

  else if (field.flags[Field::Flag::IsNullable] && !ignore_nullable_flag) {
    auto* nullable = static_cast<Nullable<void>*>(data);
    if (nullable->data) {
      util::RelocateWithSize(buffer, nullable->data, field.data_size);
      RelocateFieldData(nullable->data, field, buffer, ignore_array_flag, true);
    }
  }

  else if (field.type == Field::Type::Struct) {
    for (const ResField& subfield : field.GetFields()) {
      RelocateFieldData(reinterpret_cast<void*>(uintptr_t(data) + subfield.offset_in_value),
                        subfield, buffer);
    }
  }
}
}  // namespace

Sheet::Sheet(tcb::span<u8> data) : m_data{data} {
  if (data.size() < sizeof(ResHeader))
    throw InvalidDataError("Invalid header");

  ResHeader& header = GetHeader();
  if (header.magic != util::MakeMagic("gsht"))
    throw InvalidDataError("Invalid magic");
  if (header.version != 1)
    throw InvalidDataError("Invalid version (expected 1)");
  if (header.bool_size != 1)
    throw InvalidDataError("Invalid bool size");
  if (header.pointer_size != 8)
    throw InvalidDataError("Invalid pointer size");
  if (!header.name)
    throw InvalidDataError("Missing name");

  // Relocate all pointers.

  util::Relocate(data, header.name);
  util::RelocateWithSize(data, header.values, header.num_values * header.value_size);

  if (data.data() + data.size() < (u8*)&GetAllFieldsRaw()[header.num_fields])
    throw std::out_of_range("Fields are out of bounds");

  for (ResField& field : GetRootFields())
    RelocateField(field, nullptr, data);

  for (void* value : GetValues()) {
    for (const ResField& field : GetRootFields()) {
      RelocateFieldData(reinterpret_cast<void*>(uintptr_t(value) + field.offset_in_value), field,
                        data);
    }
  }

  // Build the int or string map.

  const auto key_field =
      std::find_if(GetRootFields().begin(), GetRootFields().end(),
                   [](const ResField& field) { return field.flags[Field::Flag::IsKey]; });
  if (key_field != GetRootFields().end()) {
    m_key_field = &*key_field;

    if (key_field->flags[Field::Flag::IsArray] || key_field->flags[Field::Flag::IsNullable])
      throw InvalidDataError("Key fields cannot be Arrays or Nullables");

    switch (key_field->type) {
    case Field::Type::Int:
      for (void* value : GetValues()) {
        m_int_map.emplace(*reinterpret_cast<int*>(uintptr_t(value) + key_field->offset_in_value),
                          value);
      }
      break;
    case Field::Type::String:
      for (void* value : GetValues()) {
        m_string_map.emplace(
            *reinterpret_cast<const char**>(uintptr_t(value) + key_field->offset_in_value), value);
      }
      break;
    default:
      throw InvalidDataError("Key fields must be of type Int or String");
    }
  }
}

tcb::span<ResField> Sheet::GetRootFields() const {
  return {GetAllFieldsRaw(), GetHeader().num_root_fields};
}

tcb::span<ResField> Sheet::GetAllFields() const {
  return {GetAllFieldsRaw(), GetHeader().num_fields};
}

FieldMap Sheet::MakeFieldMap() const {
  return gsheet::MakeFieldMap(GetRootFields());
}

ResField::ResField() {
  // Older versions of GCC appear not to initialize padding bits in this structure
  // even when zero initialization is supposed to be done.
  // So let's memset the structure manually to be sure.
  std::memset(this, 0, sizeof(*this));
}

Field::Field(const ResField& raw) {
  name = raw.name;
  type_name = raw.type_name;
  type = raw.type;
  x11 = raw.x11;
  flags = raw.flags;
  offset_in_value = raw.offset_in_value;
  inline_size = raw.inline_size;
  data_size = raw.data_size;
  fields.reserve(raw.num_fields);
  fields.assign(raw.GetFields().begin(), raw.GetFields().end());
}

template <typename T, bool IsUniquePtr = false>
static std::vector<T> ParseValueArray(const OpaqueArray* array, const Field& field,
                                      bool ignore_nullable_flag) {
  std::vector<T> vector;
  vector.reserve(array->size);
  for (void* item : array->Items(field.data_size)) {
    if constexpr (IsUniquePtr) {
      auto ptr = std::get<std::unique_ptr<T>>(
          std::move(Data{item, field, true, ignore_nullable_flag}.v.v));
      vector.emplace_back(std::move(*ptr));
    } else {
      vector.emplace_back(
          std::get<T>(std::move(Data{item, field, true, ignore_nullable_flag}.v.v)));
    }
  }
  return vector;
}

Data::Data(const void* data, const Field& field, bool ignore_array_flag,
           bool ignore_nullable_flag) {
  if (field.flags[Field::Flag::IsArray] && !ignore_array_flag) {
    auto* array = static_cast<const OpaqueArray*>(data);
    switch (field.type) {
    case Field::Type::Struct:
      v = ParseValueArray<Struct, true>(array, field, ignore_nullable_flag);
      break;
    case Field::Type::Bool:
      v = ParseValueArray<bool>(array, field, ignore_nullable_flag);
      break;
    case Field::Type::Int:
      v = ParseValueArray<int>(array, field, ignore_nullable_flag);
      break;
    case Field::Type::Float:
      v = ParseValueArray<float>(array, field, ignore_nullable_flag);
      break;
    case Field::Type::String:
      v = ParseValueArray<std::string, true>(array, field, ignore_nullable_flag);
      break;
    default:
      throw InvalidDataError("Unexpected field type");
    }
  }

  else if (field.type == Field::Type::String) {
    auto* string = static_cast<const String*>(data);
    v = string->data ? std::string(string->data) : "";
  }

  else if (field.flags[Field::Flag::IsNullable] && !ignore_nullable_flag) {
    auto* nullable = static_cast<const Nullable<void>*>(data);
    if (nullable->data)
      *this = Data(nullable->data, field, ignore_array_flag, true);
    else
      v = Null{};
  }

  else if (field.type == Field::Type::Struct) {
    Struct struct_;
    struct_.reserve(field.fields.size());
    for (const Field& subfield : field.fields) {
      const auto* ptr = reinterpret_cast<const void*>(uintptr_t(data) + subfield.offset_in_value);
      struct_.emplace(subfield.name, Data{ptr, subfield, false, false});
    }
    v = std::move(struct_);
  } else if (field.type == Field::Type::Bool) {
    v = *static_cast<const bool*>(data);
  } else if (field.type == Field::Type::Int) {
    v = *static_cast<const int*>(data);
  } else if (field.type == Field::Type::Float) {
    v = *static_cast<const float*>(data);
  }
}

SheetRw Sheet::MakeRw() const {
  SheetRw sheet;

  sheet.alignment = GetHeader().alignment;
  sheet.hash = GetHeader().hash;
  sheet.name = GetName();

  sheet.root_fields.reserve(GetRootFields().size());
  sheet.root_fields.assign(GetRootFields().begin(), GetRootFields().end());

  sheet.values.reserve(GetHeader().num_values);
  for (const void* value : GetValues()) {
    Data::Struct struct_;
    struct_.reserve(sheet.root_fields.size());
    for (const Field& field : sheet.root_fields) {
      const auto* ptr = reinterpret_cast<const void*>(uintptr_t(value) + field.offset_in_value);
      struct_.emplace(field.name, Data{ptr, field});
    }
    sheet.values.emplace_back(std::move(struct_));
  }

  return sheet;
}

}  // namespace oead::gsheet
