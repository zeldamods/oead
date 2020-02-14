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

#include <nonstd/span.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>

#include <c4/std/string.hpp>
#include <ryml.hpp>
#include "../lib/libyaml/include/yaml.h"

#include <oead/types.h>
#include <oead/util/type_utils.h>

namespace oead::yml {

std::string FormatFloat(float value);
std::string FormatDouble(double value);

using Scalar = std::variant<std::nullptr_t, bool, u64, double, std::string>;

bool StringNeedsQuotes(std::string_view value);

enum class TagBasedType {
  Bool,
  Str,
  Int,
  Float,
  Null,
};
using TagRecognizer = std::optional<TagBasedType> (*)(std::string_view tag);

Scalar ParseScalar(std::string_view tag, std::string_view value, bool is_quoted,
                   TagRecognizer recognizer);
Scalar ParseScalar(const ryml::NodeRef& node, TagRecognizer recognizer);
Scalar ParseScalarKey(const ryml::NodeRef& node, TagRecognizer recognizer);

void InitRymlIfNeeded();

inline std::string_view RymlSubstrToStrView(c4::csubstr str) {
  return {str.data(), str.size()};
}
inline c4::csubstr StrViewToRymlSubstr(std::string_view str) {
  return {str.data(), str.size()};
}
inline std::string_view RymlGetValTag(const ryml::NodeRef& n) {
  return n.has_val_tag() ? RymlSubstrToStrView(n.val_tag()) : std::string_view{};
}
inline std::string_view RymlGetKeyTag(const ryml::NodeRef& n) {
  return n.has_key_tag() ? RymlSubstrToStrView(n.key_tag()) : std::string_view{};
}
inline ryml::NodeRef RymlGetMapItem(const ryml::NodeRef& n, std::string_view key) {
  auto child = n.is_map() ? n.find_child(StrViewToRymlSubstr(key)) : ryml::NodeRef{};
  if (n.valid())
    return child;
  throw std::out_of_range("No such key: " + std::string(key));
}

class ParseError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class RymlError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class LibyamlParser {
public:
  LibyamlParser(tcb::span<const u8> data) {
    yaml_parser_initialize(&m_parser);
    yaml_parser_set_input_string(&m_parser, data.data(), data.size());
  }
  ~LibyamlParser() { yaml_parser_delete(&m_parser); }
  LibyamlParser(const LibyamlParser&) = delete;
  LibyamlParser& operator=(const LibyamlParser&) = delete;
  operator yaml_parser_t*() { return &m_parser; }

  template <typename Callable>
  void Parse(Callable event_handler) {
    yaml_event_t event;
    bool done = false;
    while (!done) {
      if (!yaml_parser_parse(&m_parser, &event))
        throw ParseError("yaml_parser_parse failed");

      event_handler(event);

      done = event.type == YAML_STREAM_END_EVENT;
      yaml_event_delete(&event);
    }
  }

private:
  yaml_parser_t m_parser;
};

class LibyamlEmitter {
public:
  LibyamlEmitter();
  ~LibyamlEmitter();
  LibyamlEmitter(const LibyamlEmitter&) = delete;
  LibyamlEmitter& operator=(const LibyamlEmitter&) = delete;
  operator yaml_emitter_t*() { return &m_emitter; }

  void Emit(yaml_event_t& event, bool ignore_errors = false);

  void EmitScalar(std::string_view value, bool plain_implicit, bool quoted_implicit,
                  std::string_view tag = {}) {
    yaml_event_t event;
    const auto style = value.empty() ? YAML_SINGLE_QUOTED_SCALAR_STYLE : YAML_ANY_SCALAR_STYLE;
    yaml_scalar_event_initialize(&event, nullptr, tag.empty() ? nullptr : (const u8*)tag.data(),
                                 (const u8*)value.data(), value.size(), plain_implicit,
                                 quoted_implicit, style);
    Emit(event);
  }

  void EmitNull() { EmitScalar("null", true, false); }
  void EmitBool(bool v, std::string_view tag = "!!bool") {
    EmitScalar(v ? "true" : "false", true, false, tag);
  }
  void EmitFloat(float v, std::string_view tag = "!!float") {
    EmitScalar(FormatFloat(v), true, false, tag);
  }
  void EmitDouble(double v, std::string_view tag = "!f64") {
    EmitScalar(FormatDouble(v), false, false, tag);
  }
  template <typename T = int>
  void EmitInt(T v, std::string_view tag = "!!int") {
    EmitScalar(std::to_string(v), tag == "!!int", false, tag);
  }
  void EmitString(std::string_view v, std::string_view tag) { EmitScalar(v, false, false, tag); }
  void EmitString(std::string_view v) { EmitScalar(v, !StringNeedsQuotes(v), true); }

  /// Emits an inline sequence of bools, ints or floats.
  template <typename T>
  void EmitSimpleSequence(tcb::span<const T> sequence, std::string_view sequence_tag = {}) {
    yaml_event_t event;
    yaml_sequence_start_event_initialize(&event, nullptr, (const u8*)sequence_tag.data(),
                                         sequence_tag.empty(), YAML_FLOW_SEQUENCE_STYLE);
    Emit(event);
    for (const T& v : sequence) {
      if constexpr (std::is_same_v<T, bool>)
        EmitBool(v);
      else if constexpr (std::is_same_v<typename NumberType<T>::type, double>)
        EmitDouble(v);
      else if constexpr (std::is_same_v<typename NumberType<T>::type, float>)
        EmitFloat(v);
      else if constexpr (std::is_integral_v<typename NumberType<T>::type>)
        EmitInt(v);
      else
        static_assert(util::AlwaysFalse<T>(), "Unsupported type!");
    }
    yaml_sequence_end_event_initialize(&event);
    Emit(event);
  }

  template <typename T>
  void EmitSimpleSequence(std::initializer_list<T> l, std::string_view tag = {}) {
    EmitSimpleSequence(tcb::span<const T>(l.begin(), l.end()), tag);
  }

  template <typename... Ts>
  void EmitSimpleSequence(const std::tuple<Ts...>& tuple, std::string_view tag = {}) {
    std::apply([&](auto&&... args) { EmitSimpleSequence({args...}, tag); }, tuple);
  }

  struct MappingScope {
    MappingScope(LibyamlEmitter& emitter_, std::string_view tag, yaml_mapping_style_t style)
        : emitter{emitter_} {
      yaml_event_t event;
      yaml_mapping_start_event_initialize(&event, nullptr, (const u8*)tag.data(),
                                          tag.empty() ? 1 : 0, style);
      emitter.Emit(event);
    }

    ~MappingScope() {
      yaml_event_t event;
      yaml_mapping_end_event_initialize(&event);
      // Ignore errors here because this shouldn't throw...
      emitter.Emit(event, true);
    }

  private:
    LibyamlEmitter& emitter;
  };

protected:
  yaml_emitter_t m_emitter;
};

template <typename Container>
class LibyamlEmitterWithStorage : public LibyamlEmitter {
public:
  LibyamlEmitterWithStorage() : LibyamlEmitter{} {
    const auto write_handler = [](void* userdata, u8* buffer, size_t size) {
      auto* self = static_cast<LibyamlEmitterWithStorage*>(userdata);
      self->m_output.insert(self->m_output.end(), (char*)buffer, (char*)buffer + size);
      return 1;
    };
    yaml_emitter_set_output(&m_emitter, write_handler, this);
  }

  ~LibyamlEmitterWithStorage() = default;

  Container& GetOutput() { return m_output; }
  const Container& GetOutput() const { return m_output; }

private:
  Container m_output;
};

}  // namespace oead::yml
