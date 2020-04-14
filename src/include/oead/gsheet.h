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

#include <absl/container/flat_hash_map.h>
#include <memory>
#include <nonstd/span.h>
#include <string>
#include <string_view>
#include <vector>

#include <easy_iterator.h>

#include <oead/types.h>
#include <oead/util/binary_reader.h>
#include <oead/util/bit_utils.h>
#include <oead/util/magic_utils.h>
#include <oead/util/variant_utils.h>

namespace oead::gsheet {

constexpr auto Magic = util::MakeMagic("gsht");

struct ResHeader {
  std::array<char, 4> magic = Magic;
  int version = 1;
  /// Unknown - probably some kind of hash or ID?
  u32 hash = 0;
  u8 bool_size = 1;
  u8 pointer_size = 8;
  u8 alignment = 8;
  const char* name;
  u32 num_root_fields;
  u32 num_fields;
  void* values;
  u32 num_values;
  u32 value_size;
};
static_assert(sizeof(ResHeader) == 0x30);

struct ResField;

/// Grezzo datasheet field.
struct Field {
  enum class Type : u8 {
    /// C/C++ style structure.
    Struct = 0,
    /// Boolean.
    Bool = 1,
    /// Signed 32-bit integer.
    Int = 2,
    /// Single-precision floating point number (binary32).
    Float = 3,
    /// Null-terminated string.
    String = 4,
  };

  enum class Flag : u16 {
    IsNullable = 1 << 0,
    IsArray = 1 << 1,
    IsKey = 1 << 2,
    Unknown3 = 1 << 3,
    IsEnum = 1 << 4,
    Unknown5 = 1 << 5,
  };

  Field() = default;
  Field(const ResField& raw);

  /// Name (must not be empty).
  std::string name;
  /// Type name.
  std::string type_name;
  /// Field type.
  Type type;
  /// Unknown; depth level?
  u8 x11;
  /// Flags.
  util::Flags<Flag> flags;
  /// Offset of this field in the value structure.
  u16 offset_in_value;
  /// Size of this field in the value structure.
  /// For strings and arrays, this is always 0x10.
  u16 inline_size;
  /// Size of the field data.
  /// For strings and inline types (inline structs, ints, floats, bools), this is the same as the
  /// size in the value structure.
  u16 data_size;
  /// [For structs] Fields
  std::vector<Field> fields;

  constexpr auto ReflectionFields() const {
    return std::tie(name, type_name, type, x11, flags, offset_in_value, inline_size, data_size,
                    fields);
  }
  friend bool operator==(const Field& a, const Field& b) {
    return a.ReflectionFields() == b.ReflectionFields();
  }
};

/// Grezzo datasheet field (serialized).
struct ResField {
  using Type = Field::Type;
  using Flag = Field::Flag;

  ResField();

  tcb::span<ResField> GetFields() const { return {fields, num_fields}; }

  /// Name (guaranteed to be non-null).
  const char* name;
  /// Type name.
  const char* type_name;
  /// Field type.
  Type type;
  /// Unknown; depth level?
  u8 x11;
  /// Flags.
  util::Flags<Flag> flags;
  /// Offset of this field in the value structure.
  u16 offset_in_value;
  /// Size of this field in the value structure.
  /// For strings and arrays, this is always 0x10.
  u16 inline_size;
  /// Size of the field data.
  /// For strings and inline types (inline structs, ints, floats, bools), this is the same as the
  /// size in the value structure.
  u16 data_size;
  /// [For structs] Number of fields
  u16 num_fields;
  /// [For structs] Fields
  ResField* fields;
  /// [For structs] Parent field (filled in during parsing; always 0xdeadbeefdeadbeef when
  /// serialized)
  ResField* parent;
};
static_assert(sizeof(ResField) == 0x30);

/// For defining datasheet structures.
template <typename T>
struct Nullable {
  /// Nullptr if no value.
  T* data = nullptr;
};

/// For defining datasheet structures.
template <typename T>
struct Array {
  tcb::span<T> Span() const { return {data, size}; }
  operator tcb::span<T>() const { return Span(); }

  T* data = nullptr;
  u32 size = 0;

private:
  [[maybe_unused]] u32 padding = 0;
};

/// For defining datasheet structures.
struct String {
  std::string_view Str() const { return {data, size}; }
  operator std::string_view() const { return Str(); }

  const char* data = nullptr;
  u32 size = 0;

private:
  [[maybe_unused]] u32 padding = 0;
};

using FieldMap = absl::flat_hash_map<std::string_view, ResField*>;
FieldMap MakeFieldMap(tcb::span<ResField> fields);

/// Represents a piece of field data in a datasheet.
struct Data {
  enum class Type {
    Struct,
    Bool,
    Int,
    Float,
    String,
    StructArray,
    BoolArray,
    IntArray,
    FloatArray,
    StringArray,
    Null,
  };

  using Null = std::nullptr_t;
  /// A struct is represented as a string to Value map, with the field names as keys.
#ifdef OEAD_GSHEET_DEBUG
  using Struct = std::unordered_map<std::string, Data>;
#else
  using Struct = absl::flat_hash_map<std::string, Data>;
#endif
  using Variant = util::Variant<Type,                                       //
                                std::unique_ptr<Struct>,                    //
                                bool,                                       //
                                int,                                        //
                                float,                                      //
                                std::unique_ptr<std::string>,               //
                                std::unique_ptr<std::vector<Struct>>,       //
                                std::unique_ptr<std::vector<bool>>,         //
                                std::unique_ptr<std::vector<int>>,          //
                                std::unique_ptr<std::vector<float>>,        //
                                std::unique_ptr<std::vector<std::string>>,  //
                                Null                                        //
                                >;

  Data() : v{Null{}} {}
  Data(const void* raw, const Field& field, bool ignore_array_flag = false,
       bool ignore_nullable_flag = false);

  template <typename T, std::enable_if_t<std::is_constructible_v<Variant, T>>* = nullptr>
  Data(T value) : v{std::move(value)} {}

  Data(const Data& other) { *this = other; }
  Data(Data&& other) noexcept { *this = std::move(other); }
  Data& operator=(const Data& other) = default;
  Data& operator=(Data&& other) noexcept = default;

  constexpr bool IsNull() const { return std::holds_alternative<Null>(v.v); }
  constexpr bool IsArray() const {
    return Type::StructArray <= v.GetType() && v.GetType() <= Type::StringArray;
  }

  template <typename Callable>
  auto VisitArray(Callable&& fn) const {
    if (auto* x = std::get_if<std::unique_ptr<std::vector<Struct>>>(&v.v))
      return fn(*x->get());
    if (auto* x = std::get_if<std::unique_ptr<std::vector<bool>>>(&v.v))
      return fn(*x->get());
    if (auto* x = std::get_if<std::unique_ptr<std::vector<int>>>(&v.v))
      return fn(*x->get());
    if (auto* x = std::get_if<std::unique_ptr<std::vector<float>>>(&v.v))
      return fn(*x->get());
    if (auto* x = std::get_if<std::unique_ptr<std::vector<std::string>>>(&v.v))
      return fn(*x->get());
    throw std::logic_error("Not an array");
  }

  OEAD_DEFINE_FIELDS(Data, v)

  Variant v;
};
static_assert(sizeof(Data) == 0x10);

/// An iterator for opaque blobs for which only the element size is known.
class OpaqueIterator : public easy_iterator::InitializedIterable {
public:
  OpaqueIterator(void* begin, void* end, size_t item_size)
      : m_current{begin}, m_end{end}, m_item_size{item_size} {}

  bool advance() {
    m_current = reinterpret_cast<void*>(uintptr_t(m_current) + m_item_size);
    return m_current != m_end;
  }
  bool init() { return m_current != m_end; }
  void* value() { return m_current; }

private:
  void* m_current;
  void* m_end;
  size_t m_item_size;
};

/// Grezzo datasheet.
///
/// This allows reading and writing binary datasheets and data modifications.
/// For a readonly parser, see the Sheet class.
struct SheetRw {
  /// Serialize the datasheet to the v1 binary format.
  std::vector<u8> ToBinary() const;

  u8 alignment = 8;
  u32 hash = 0;
  std::string name;
  std::vector<Field> root_fields;
  std::vector<Data::Struct> values;
};

/// Grezzo datasheet.
///
/// To actually access values that are stored in a binary datasheet, users are intended to define
/// C++ structures and cast value pointers to the appropriate structure type.
///
/// See also SheetRw for a version of this class that allows for reflection and modifications.
class Sheet {
public:
  Sheet(tcb::span<u8> data);

  ResHeader& GetHeader() const { return *reinterpret_cast<ResHeader*>(m_data.data()); }
  std::string_view GetName() const { return GetHeader().name; }

  /// Get the datasheet root fields.
  tcb::span<ResField> GetRootFields() const;

  /// Get every single datasheet field (including nested fields).
  tcb::span<ResField> GetAllFields() const;

  /// Get the datasheet values (as an iterable).
  auto GetValues() const {
    const auto& header = GetHeader();
    return easy_iterator::MakeIterable<OpaqueIterator>(
        header.values,
        reinterpret_cast<void*>((uintptr_t)header.values + header.value_size * header.num_values),
        header.value_size);
  }

  using IntMap = absl::flat_hash_map<int, void*>;
  using StringMap = absl::flat_hash_map<std::string_view, void*>;

  const IntMap& GetIntMap() const { return m_int_map; }
  const StringMap& GetStringMap() const { return m_string_map; }

  FieldMap MakeFieldMap() const;
  SheetRw MakeRw() const;

private:
  ResField* GetAllFieldsRaw() const { return reinterpret_cast<ResField*>(&GetHeader() + 1); }

  tcb::span<u8> m_data;
  /// Key field. Nullptr if there is no key field.
  ResField* m_key_field = nullptr;
  /// Only valid if there is a valid key field and the key field type is Int.
  IntMap m_int_map;
  /// Only valid if there is a valid key field and the key field type is String.
  StringMap m_string_map;
};

}  // namespace oead::gsheet
