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
#include <concepts>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <tsl/ordered_map.h>
#include <variant>
#include <vector>

#include <oead/errors.h>
#include <oead/types.h>
#include <oead/util/string_utils.h>
#include <oead/util/type_utils.h>
#include <oead/util/variant_utils.h>

namespace tsl {

template <typename H, typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator, typename ValueTypeContainer, typename IndexType>
H AbslHashValue(
    H hash_state,
    const ordered_map<Key, T, Hash, KeyEqual, Allocator, ValueTypeContainer, IndexType>& map) {
  for (const auto& t : map) {
    hash_state = H::combine(std::move(hash_state), t);
  }
  return H::combine(std::move(hash_state), map.size());
}

}  // namespace tsl

namespace oead {

/// BYML value class. This represents a generic value (array, dict, bool, float, u32, etc.)
class Byml {
public:
  enum class Type {
    Null = 0,
    String,
    Binary,
    AlignedBinary,
    Array,
    MonoTypedArray,
    Hash,
    OrderedHash,
    Hash32,
    Hash64,
    OrderedHash32,
    OrderedHash64,
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
  using OrderedHash = tsl::ordered_map<std::string, Byml, util::StrHash, util::StrEqual,
                                       std::allocator<std::pair<std::string, Byml>>,
                                       std::vector<std::pair<std::string, Byml>>>;
  using Hash32 = absl::btree_map<u32, Byml>;
  using Hash64 = absl::btree_map<u64, Byml>;
  using OrderedHash32 =
      tsl::ordered_map<u32, Byml, absl::Hash<u32>, std::equal_to<u32>,
                       std::allocator<std::pair<u32, Byml>>, std::vector<std::pair<u32, Byml>>>;
  using OrderedHash64 =
      tsl::ordered_map<u64, Byml, absl::Hash<u64>, std::equal_to<u64>,
                       std::allocator<std::pair<u64, Byml>>, std::vector<std::pair<u64, Byml>>>;

  struct MonoTypedArray {
    MonoTypedArray() = default;

    explicit MonoTypedArray(Type type_) : type(type_) {}

    MonoTypedArray(const MonoTypedArray& other) { *this = other; }
    MonoTypedArray(MonoTypedArray&& other) { *this = std::move(other); }
    MonoTypedArray& operator=(const MonoTypedArray& other) = default;
    MonoTypedArray& operator=(MonoTypedArray&& other) = default;

    MonoTypedArray(Array&& elements_, Type type_);
    MonoTypedArray(const Array& elements_, Type type_);

    std::vector<Byml> elements;
    Type type = Type::Null;

    OEAD_DEFINE_FIELDS(MonoTypedArray, elements, type);
  };

  struct AlignedBinary {
    std::vector<u8> data;
    std::size_t align = 1;

    OEAD_DEFINE_FIELDS(AlignedBinary, data, align);
  };

  using Value = util::Variant<Type, Null, std::unique_ptr<String>, std::unique_ptr<std::vector<u8>>,
                              std::unique_ptr<AlignedBinary>, std::unique_ptr<Array>,
                              std::unique_ptr<MonoTypedArray>, std::unique_ptr<Hash>,
                              std::unique_ptr<OrderedHash>, std::unique_ptr<Hash32>,
                              std::unique_ptr<Hash64>, std::unique_ptr<OrderedHash32>,
                              std::unique_ptr<OrderedHash64>, bool, S32, F32, U32, S64, U64, F64>;

  Byml() = default;
  Byml(const Byml& other) { *this = other; }
  Byml(Byml&& other) noexcept { *this = std::move(other); }
  template <typename T>
    requires std::constructible_from<Value, T>
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
    T* operator->() {
      if (auto ptr = std::get_if<0>(&m_storage))
        return ptr;
      if (auto ptr = std::get_if<1>(&m_storage))
        return &ptr->get();
      return nullptr;
    }
    const T* operator->() const {
      if (auto ptr = std::get_if<0>(&m_storage))
        return ptr;
      if (auto ptr = std::get_if<1>(&m_storage))
        return &ptr->get();
      return nullptr;
    }
    T& operator*() { return *operator->(); }
    const T& operator*() const { return *operator->(); }

  private:
    using Storage = std::variant<T, std::reference_wrapper<T>>;
    Storage m_storage;
  };

  template <Type Type, typename T>
  Reference<const T> Get(std::string_view key, T default_) const {
    if (GetType() == Byml::Type::Hash) {
      auto it = GetHash().find(key);
      if (it == GetHash().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash().find(key);
      if (it == GetOrderedHash().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    }
  }
  template <Type Type, typename T>
  Reference<T> Get(std::string_view key, T default_) {
    if (GetType() == Byml::Type::Hash) {
      auto it = GetHash().find(key);
      if (it == GetHash().end())
        return default_;
      return std::ref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash().find(key);
      if (it == GetOrderedHash().end())
        return default_;
      //  This is hacky, but tsl::ordered_map's iterator does not provide a non-const view of the
      //  data (iterator.operator->() or iterator.operator*() is always const)
      return std::ref(const_cast<T&>(it->second.Get<Type>()));
    }
  }
  template <Type Type, typename T>
  Reference<const T> Get(u32 key, T default_) const {
    if (GetType() == Byml::Type::Hash32) {
      auto it = GetHash32().find(key);
      if (it == GetHash32().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash32().find(key);
      if (it == GetOrderedHash32().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    }
  }
  template <Type Type, typename T>
  Reference<T> Get(u32 key, T default_) {
    if (GetType() == Byml::Type::Hash32) {
      auto it = GetHash32().find(key);
      if (it == GetHash32().end())
        return default_;
      return std::ref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash32().find(key);
      if (it == GetOrderedHash32().end())
        return default_;
      return std::ref(const_cast<T&>(it->second.Get<Type>()));
    }
  }
  template <Type Type, typename T>
  Reference<const T> Get(u64 key, T default_) const {
    if (GetType() == Byml::Type::Hash64) {
      auto it = GetHash64().find(key);
      if (it == GetHash64().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash64().find(key);
      if (it == GetOrderedHash64().end())
        return default_;
      return std::cref(it->second.Get<Type>());
    }
  }
  template <Type Type, typename T>
  Reference<T> Get(u64 key, T default_) {
    if (GetType() == Byml::Type::Hash64) {
      auto it = GetHash64().find(key);
      if (it == GetHash64().end())
        return default_;
      return std::ref(it->second.Get<Type>());
    } else {
      auto it = GetOrderedHash64().find(key);
      if (it == GetOrderedHash64().end())
        return default_;
      return std::ref(const_cast<T&>(it->second.Get<Type>()));
    }
  }

  /// Load a document from binary data.
  static Byml FromBinary(std::span<const u8> data);
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

  Hash& GetHash();
  OrderedHash& GetOrderedHash();
  Hash32& GetHash32();
  Hash64& GetHash64();
  OrderedHash32& GetOrderedHash32();
  OrderedHash64& GetOrderedHash64();
  Array& GetArray();
  MonoTypedArray& GetMonoTypedArray();
  String& GetString();
  std::vector<u8>& GetBinary();
  AlignedBinary& GetAlignedBinary();
  const Hash& GetHash() const;
  const OrderedHash& GetOrderedHash() const;
  const Hash32& GetHash32() const;
  const Hash64& GetHash64() const;
  const OrderedHash32& GetOrderedHash32() const;
  const OrderedHash64& GetOrderedHash64() const;
  const Array& GetArray() const;
  const MonoTypedArray& GetMonoTypedArray() const;
  const String& GetString() const;
  const std::vector<u8>& GetBinary() const;
  const AlignedBinary& GetAlignedBinary() const;
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
