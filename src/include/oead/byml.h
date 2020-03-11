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

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>
#include <functional>
#include <memory>
#include <nonstd/span.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <oead/errors.h>
#include <oead/types.h>
#include <oead/util/type_utils.h>
#include <oead/util/variant_utils.h>

namespace oead {

/// BYML value class. This represents a generic value (array, dict, bool, float, u32, etc.)
class Byml {
public:
  enum class Type {
    Null = 0,
    String,
    Array,
    Hash,
    Bool,
    Int,
    Float,
    UInt,
    Int64,
    UInt64,
    Double,
  };

  using Null = std::nullptr_t;
  using String = std::string;
  using Array = std::vector<Byml>;
  using Hash = absl::btree_map<std::string, Byml>;

  using Value = util::Variant<Type, Null, std::unique_ptr<String>, std::unique_ptr<Array>,
                              std::unique_ptr<Hash>, bool, S32, F32, U32, S64, U64, F64>;

  Byml() = default;
  Byml(const Byml& other) { *this = other; }
  Byml(Byml&& other) noexcept { *this = std::move(other); }
  template <typename T, std::enable_if_t<std::is_constructible_v<Value, T>>* = nullptr>
  Byml(T value) : m_value{std::move(value)} {}
  Byml& operator=(const Byml& other) = default;
  Byml& operator=(Byml&& other) noexcept = default;

  OEAD_DEFINE_FIELDS(Byml, m_value);

  Type GetType() const { return m_value.GetType(); }
  template <Type type>
  const auto& Get() const {
    return m_value.Get<type>();
  }
  template <Type type>
  auto& Get() {
    return m_value.Get<type>();
  }
  auto& GetVariant() { return m_value; }
  const auto& GetVariant() const { return m_value; }

  /// Get an item from a map/hash.
  template <typename T>
  struct Reference {
    Reference(T value) : m_storage{std::move(value)} {}
    Reference(std::reference_wrapper<T> value) : m_storage{std::move(value)} {}
    T* operator->() const {
      if (auto ptr = std::get_if<0>(&m_storage))
        return ptr;
      if (auto ptr = std::get_if<1>(&m_storage))
        return &ptr->get();
      return nullptr;
    }
    T& operator*() const { return *operator->(); }

  private:
    using Storage = std::variant<T, std::reference_wrapper<T>>;
    Storage m_storage;
  };

  template <Type Type, typename T>
  Reference<const T> Get(std::string_view key, T default_) const {
    auto it = GetHash().find(key);
    if (it == GetHash().end())
      return default_;
    return std::cref(it->second.Get<Type>());
  }
  template <Type Type, typename T>
  Reference<T> Get(std::string_view key, T default_) {
    auto it = GetHash().find(key);
    if (it == GetHash().end())
      return default_;
    return std::ref(it->second.Get<Type>());
  }

  /// Load a document from binary data.
  static Byml FromBinary(tcb::span<const u8> data);
  /// Load a document from YAML text.
  static Byml FromText(std::string_view yml_text);

  /// Serialize the document to BYML with the specified endianness and version number.
  /// This can only be done for Null, Array or Hash nodes.
  std::vector<u8> ToBinary(bool big_endian, int version = 2) const;
  /// Serialize the document to YAML.
  /// This can only be done for Null, Array or Hash nodes.
  std::string ToText() const;

  // These getters mirror the behaviour of Nintendo's BYML library.
  // Some of them will perform type conversions automatically.
  // If value types are incorrect, an exception is thrown.

  const Hash& GetHash() const;
  const Array& GetArray() const;
  const String& GetString() const;
  bool GetBool() const;
  s32 GetInt() const;
  u32 GetUInt() const;
  f32 GetFloat() const;
  s64 GetInt64() const;
  u64 GetUInt64() const;
  f64 GetDouble() const;

private:
  Value m_value;
};

}  // namespace oead
