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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>
#include <array>
#include <memory>
#include <nonstd/span.h>
#include <string>
#include <string_view>
#include <tsl/ordered_map.h>
#include <variant>
#include <vector>

#include <oead/types.h>
#include <oead/util/hash.h>
#include <oead/util/variant_utils.h>

namespace oead::aamp {

/// A table of names that is used to recover original names in binary parameter archives
/// which store only name hashes.
struct NameTable {
  NameTable(bool with_botw_strings = false);

  /// Tries to guess the name that is associated with the given hash and index
  /// (of the parameter / object / list in its parent).
  ///
  /// The table is automatically updated with any newly found names if an indice-based guess
  /// was necessary.
  std::optional<std::string_view> GetName(u32 hash, int index, u32 parent_name_hash);

  /// Add a known string to the name table.
  /// \return a view to the added string.
  std::string_view AddName(std::string name) {
    const u32 hash = util::crc32(name);
    return AddName(hash, std::move(name));
  }
  /// Add a known string to the name table. This should be used if the string's hash
  /// has already been computed in order to avoid recomputing it.
  /// \return a view to the added string.
  std::string_view AddName(u32 hash, std::string name);
  /// Add a known string to the name table.
  /// \warning Since this is taking a string view, the actual string data must outlive this table.
  void AddNameReference(std::string_view name);

  /// Hash to name map. The strings are only references.
  absl::flat_hash_map<u32, std::string_view> names;
  /// Hash to name map. The strings are owned.
  absl::flat_hash_map<u32, std::string> owned_names;
  /// List of numbered names (i.e. names that contain a printf specifier for the index).
  std::vector<std::string_view> numbered_names;
};

/// Returns the default instance of the name table, which is automatically populated with
/// Breath of the Wild strings.
/// Initialised on first use.
NameTable& GetDefaultNameTable();

/// Parameter structure name. This is a wrapper around a CRC32 hash.
struct Name {
  constexpr Name(std::string_view name) : hash{util::crc32(name)} {}
  constexpr Name(const char* name) : hash{util::crc32(name)} {}
  constexpr Name(u32 name_crc32) : hash{name_crc32} {}
  operator u32() const { return hash; }
  /// The CRC32 hash of the name.
  u32 hash;

  OEAD_DEFINE_FIELDS(Name, hash);
};

/// Parameter.
///
/// Note that unlike agl::utl::Parameter the name is not stored as part of the parameter class
/// in order to make the parameter logic simpler and more efficient.
class Parameter {
public:
  enum class Type : u8 {
    Bool = 0,
    F32,
    Int,
    Vec2,
    Vec3,
    Vec4,
    Color,
    String32,
    String64,
    Curve1,
    Curve2,
    Curve3,
    Curve4,
    BufferInt,
    BufferF32,
    String256,
    Quat,
    U32,
    BufferU32,
    BufferBinary,
    StringRef,
  };

  using Value =
      util::Variant<Type, bool, float, int, Vector2f, Vector3f, Vector4f, Color4f,
                    std::unique_ptr<FixedSafeString<32>>, std::unique_ptr<FixedSafeString<64>>,
                    std::unique_ptr<std::array<Curve, 1>>, std::unique_ptr<std::array<Curve, 2>>,
                    std::unique_ptr<std::array<Curve, 3>>, std::unique_ptr<std::array<Curve, 4>>,
                    std::unique_ptr<std::vector<int>>, std::unique_ptr<std::vector<float>>,
                    std::unique_ptr<FixedSafeString<256>>, Quatf, U32,
                    std::unique_ptr<std::vector<u32>>, std::unique_ptr<std::vector<u8>>,
                    std::unique_ptr<std::string>>;

  Parameter() = default;
  Parameter(const Parameter& other) { *this = other; }
  Parameter(Parameter&& other) noexcept { *this = std::move(other); }
  template <typename T, std::enable_if_t<std::is_constructible_v<Value, T>>* = nullptr>
  Parameter(T value) : m_value{std::move(value)} {}
  Parameter(F32 value) : m_value{static_cast<f32>(value)} {}
  Parameter& operator=(const Parameter& other) = default;
  Parameter& operator=(Parameter&& other) noexcept = default;

  OEAD_DEFINE_FIELDS(Parameter, m_value);

  Type GetType() const { return m_value.GetType(); }
  template <Type type>
  const auto& Get() const {
    return m_value.Get<type>();
  }
  template <Type type>
  auto& Get() {
    return m_value.Get<type>();
  }
  /// Get the value as a string view. Throws a TypeError if the parameter is not a string.
  std::string_view GetStringView() const;
  auto& GetVariant() { return m_value; }
  const auto& GetVariant() const { return m_value; }

private:
  Value m_value;
};

static_assert(sizeof(Parameter) <= 0x18);

constexpr bool IsStringType(Parameter::Type type) {
  return type == Parameter::Type::String32 || type == Parameter::Type::String64 ||
         type == Parameter::Type::String256 || type == Parameter::Type::StringRef;
}

constexpr bool IsBufferType(Parameter::Type type) {
  return type == Parameter::Type::BufferInt || type == Parameter::Type::BufferU32 ||
         type == Parameter::Type::BufferF32 || type == Parameter::Type::BufferBinary;
}

using ParameterMap = tsl::ordered_map<Name, Parameter, absl::Hash<Name>, std::equal_to<Name>,
                                      std::allocator<std::pair<Name, Parameter>>,
                                      std::vector<std::pair<Name, Parameter>>>;

/// Parameter object. This is essentially a dictionary of parameters.
struct ParameterObject {
  ParameterMap params;
  OEAD_DEFINE_FIELDS(ParameterObject, params);
};

using ParameterObjectMap =
    tsl::ordered_map<Name, ParameterObject, absl::Hash<Name>, std::equal_to<Name>,
                     std::allocator<std::pair<Name, ParameterObject>>,
                     std::vector<std::pair<Name, ParameterObject>>>;

/// Parameter list. This is essentially a dictionary of parameter objects
/// and a dictionary of parameter lists.
struct ParameterList {
  ParameterObjectMap objects;
  tsl::ordered_map<Name, ParameterList, absl::Hash<Name>, std::equal_to<Name>,
                   std::allocator<std::pair<Name, ParameterList>>,
                   std::vector<std::pair<Name, ParameterList>>>
      lists;
  OEAD_DEFINE_FIELDS(ParameterList, objects, lists);
};

using ParameterListMap =
    tsl::ordered_map<Name, ParameterList, absl::Hash<Name>, std::equal_to<Name>,
                     std::allocator<std::pair<Name, ParameterList>>,
                     std::vector<std::pair<Name, ParameterList>>>;

/// Parameter IO. This is the root parameter list and the only structure that can be serialized to
/// or deserialized from a binary parameter archive.
struct ParameterIO : ParameterList {
  static constexpr Name ParamRootKey = Name("param_root");

  /// Data version (not the AAMP format version). Typically 0.
  u32 version = 0;
  /// Data type identifier. Typically "xml".
  std::string type;

  OEAD_DEFINE_FIELDS(ParameterIO, objects, lists, version, type);

  /// Load a ParameterIO from a binary parameter archive.
  static ParameterIO FromBinary(tcb::span<const u8> data);
  /// Load a ParameterIO from a YAML representation.
  static ParameterIO FromText(std::string_view yml_text);

  /// Serialize the ParameterIO to a binary parameter archive.
  std::vector<u8> ToBinary() const;
  /// Serialize the ParameterIO to a YAML representation.
  std::string ToText() const;
};

}  // namespace oead::aamp
