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

#include "yaml.h"

#include <absl/strings/match.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_format.h>
#include <mutex>

#include <c4/error.hpp>
#include <ryml.hpp>

#include <oead/util/iterator_utils.h>

namespace oead::yml {

using namespace std::string_view_literals;

std::string FormatFloat(float value) {
  std::string repr = absl::StrFormat("%.9g", value);
  if (!absl::StrContains(repr, ".") && !absl::StrContains(repr, "e"))
    repr += ".0"sv;
  return repr;
}

std::string FormatDouble(double value) {
  std::string repr = absl::StrFormat("%.17g", value);
  if (!absl::StrContains(repr, ".") && !absl::StrContains(repr, "e"))
    repr += ".0"sv;
  return repr;
}

static bool IsInfinity(std::string_view input) {
  return util::IsAnyOf(input, ".inf", ".Inf", ".INF") ||
         util::IsAnyOf(input, "+.inf", "+.Inf", "+.INF");
}

static bool IsNegativeInfinity(std::string_view input) {
  return util::IsAnyOf(input, "-.inf", "-.Inf", "-.INF");
}

static bool IsNaN(std::string_view input) {
  return util::IsAnyOf(input, ".nan", ".NaN", ".NAN");
}

static std::optional<TagBasedType> GetTagBasedType(const std::string_view tag,
                                                   TagRecognizer recognizer) {
  if (tag.empty())
    return std::nullopt;
  if (tag == "tag:yaml.org,2002:str")
    return TagBasedType::Str;
  if (tag == "tag:yaml.org,2002:float")
    return TagBasedType::Float;
  if (tag == "tag:yaml.org,2002:int")
    return TagBasedType::Int;
  if (tag == "tag:yaml.org,2002:bool")
    return TagBasedType::Bool;
  if (tag == "tag:yaml.org,2002:null")
    return TagBasedType::Null;
  if (const auto type = recognizer(tag))
    return type;
  return std::nullopt;
}

// Deliberately not compliant to the YAML 1.2 standard to get rid of unused features
// that harm performance.
Scalar ParseScalar(const std::string_view tag, const std::string_view value, bool is_quoted,
                   TagRecognizer recognizer) {
  const auto tag_type = GetTagBasedType(tag, recognizer);

  if (tag_type == TagBasedType::Bool || util::IsAnyOf(value, "true", "false"))
    return value[0] == 't';

  // Floating-point conversions.
  const bool is_possible_double = absl::StrContains(value, ".");
  if (tag_type == TagBasedType::Float || (!tag_type && is_possible_double && !is_quoted)) {
    if (IsInfinity(value))
      return std::numeric_limits<double>::infinity();
    if (IsNegativeInfinity(value))
      return -std::numeric_limits<double>::infinity();
    if (IsNaN(value))
      return std::numeric_limits<double>::quiet_NaN();
    double maybe_double;
    if (absl::SimpleAtod(value, &maybe_double))
      return maybe_double;
    if (tag_type == TagBasedType::Float)
      throw ParseError("Failed to parse value that was explicitly marked as float");
  }

  // Integer conversions. Not YAML 1.2 compliant: base 8 is not supported as it's not useful.
  if (tag_type == TagBasedType::Int || (!tag_type && !value.empty() && !is_quoted)) {
    char* int_str_end = nullptr;
    const u64 maybe_u64 = std::strtoull(value.data(), &int_str_end, 0);
    if (value.data() + value.size() == int_str_end)
      return maybe_u64;
    if (tag_type == TagBasedType::Int)
      throw ParseError("Failed to parse value that was explicitly marked as integer");
  }

  if (tag_type == TagBasedType::Null || value == "null")
    return nullptr;

  // Fall back to treating the value as a string.
  return std::string(value);
}

bool StringNeedsQuotes(const std::string_view value) {
  if (util::IsAnyOf(value, "true", "false"))
    return true;

  const bool is_possible_double = absl::StrContains(value, ".");
  if (is_possible_double) {
    if (IsInfinity(value) || IsNegativeInfinity(value) || IsNaN(value))
      return true;
    double maybe_double;
    if (absl::SimpleAtod(value, &maybe_double))
      return true;
  }

  if (!value.empty()) {
    char* int_str_end = nullptr;
    std::strtoull(value.data(), &int_str_end, 0);
    if (value.data() + value.size() == int_str_end)
      return true;
  }

  if (value == "null")
    return true;

  return false;
}

Scalar ParseScalar(const ryml::NodeRef& node, TagRecognizer recognizer) {
  const std::string_view tag = RymlGetValTag(node);
  const std::string_view value = RymlSubstrToStrView(node.val());
  const char* arena_start = node.tree()->arena().data();
  bool is_quoted = false;
  if (node.val().data() != arena_start)
    is_quoted = util::IsAnyOf(*(node.val().data() - 1), '\'', '"');
  return ParseScalar(tag, value, is_quoted, recognizer);
}

Scalar ParseScalarKey(const ryml::NodeRef& node, TagRecognizer recognizer) {
  const std::string_view tag = RymlGetKeyTag(node);
  const std::string_view value = RymlSubstrToStrView(node.key());
  const char* arena_start = node.tree()->arena().data();
  bool is_quoted = false;
  if (node.key().data() != arena_start)
    is_quoted = util::IsAnyOf(*(node.key().data() - 1), '\'', '"');
  return ParseScalar(tag, value, is_quoted, recognizer);
}

void InitRymlIfNeeded() {
  static std::once_flag s_flag;
  std::call_once(s_flag, [] {
    ryml::Callbacks callbacks = ryml::get_callbacks();
    callbacks.m_error = [](const char* msg, size_t msg_len, void*) {
      throw RymlError("RymlError: " + std::string(msg, msg_len));
    };
    ryml::set_callbacks(callbacks);
    c4::set_error_callback([](const char* msg, size_t msg_size) {
      throw RymlError("RymlError (c4): " + std::string(msg, msg_size));
    });
  });
}

LibyamlEmitter::LibyamlEmitter() {
  yaml_emitter_initialize(&m_emitter);
  yaml_emitter_set_unicode(&m_emitter, 1);
}

LibyamlEmitter::~LibyamlEmitter() {
  yaml_emitter_delete(&m_emitter);
}

void LibyamlEmitter::Emit(yaml_event_t& event, bool ignore_errors) {
  if (!yaml_emitter_emit(&m_emitter, &event) && !ignore_errors) {
    throw std::runtime_error("Emit failed: " +
                             std::string(m_emitter.problem ? m_emitter.problem : "unknown"));
  }
}

}  // namespace oead::yml
