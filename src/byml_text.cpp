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

#include <absl/algorithm/container.h>
#include <absl/strings/str_format.h>

#include <c4/std/string.hpp>
#include <ryml.hpp>
#include "../lib/libyaml/include/yaml.h"

#include <oead/byml.h>
#include <oead/util/iterator_utils.h>
#include <oead/util/type_utils.h>
#include <oead/util/variant_utils.h>
#include "yaml.h"

namespace oead {

namespace byml {
static std::optional<yml::TagBasedType> RecognizeTag(const std::string_view tag) {
  if (util::IsAnyOf(tag, "!f64"))
    return yml::TagBasedType::Float;
  if (util::IsAnyOf(tag, "!u", "!l", "!ul"))
    return yml::TagBasedType::Int;
  return std::nullopt;
}

static Byml ScalarToValue(std::string_view tag, yml::Scalar&& scalar) {
  return util::Match(
      std::move(scalar), [](std::nullptr_t) -> Byml { return Byml::Null{}; },
      [](bool value) -> Byml { return value; },
      [](std::string&& value) -> Byml { return std::move(value); },
      [&](u64 value) -> Byml {
        if (tag == "!u")
          return U32(value);
        if (tag == "!l")
          return S64(value);
        if (tag == "!ul")
          return U64(value);
        return S32(value);
      },
      [&](f64 value) -> Byml {
        if (tag == "!f64")
          return F64(value);
        return F32(value);
      });
}

static bool ShouldUseInlineYamlStyle(const Byml& container) {
  const auto is_simple = [](const Byml& item) {
    return !util::IsAnyOf(item.GetType(), Byml::Type::Array, Byml::Type::Hash);
  };
  switch (container.GetType()) {
  case Byml::Type::Array:
    return container.GetArray().size() <= 10 && absl::c_all_of(container.GetArray(), is_simple);
  case Byml::Type::Hash:
    return container.GetHash().size() <= 10 &&
           absl::c_all_of(container.GetHash(), [&](const auto& p) { return is_simple(p.second); });
  default:
    return false;
  }
}

Byml ParseYamlNode(const c4::yml::NodeRef& node) {
  if (node.is_seq()) {
    auto array = Byml::Array{};
    array.reserve(node.num_children());
    for (const auto& child : node) {
      array.emplace_back(ParseYamlNode(child));
    }
    return array;
  }

  if (node.is_map()) {
    auto hash = Byml::Hash{};
    for (const auto& child : node) {
      std::string key{yml::RymlSubstrToStrView(child.key())};
      Byml value = ParseYamlNode(child);
      hash.emplace(std::move(key), std::move(value));
    }
    return hash;
  }

  if (node.has_val()) {
    return byml::ScalarToValue(yml::RymlGetValTag(node),
                               yml::ParseScalar(node, byml::RecognizeTag));
  }

  throw InvalidDataError("Failed to parse YAML node");
}
}  // namespace byml

Byml Byml::FromText(std::string_view yml_text) {
  yml::InitRymlIfNeeded();
  ryml::Tree tree = ryml::parse(yml::StrViewToRymlSubstr(yml_text));
  return byml::ParseYamlNode(tree.rootref());
}

std::string Byml::ToText() const {
  yml::LibyamlEmitterWithStorage<std::string> emitter;
  yaml_event_t event;

  yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
  emitter.Emit(event);

  yaml_document_start_event_initialize(&event, nullptr, nullptr, nullptr, 1);
  emitter.Emit(event);

  const auto emit = [&](auto self, const Byml& node) -> void {
    util::Match(
        node.GetVariant().v, [&](Null) { emitter.EmitNull(); },
        [&](const String& v) { emitter.EmitString(v); },
        [&](const Array& v) {
          yaml_event_t event;
          const auto style = byml::ShouldUseInlineYamlStyle(v) ? YAML_FLOW_SEQUENCE_STYLE :
                                                                 YAML_BLOCK_SEQUENCE_STYLE;
          yaml_sequence_start_event_initialize(&event, nullptr, nullptr, 1, style);
          emitter.Emit(event);

          for (const Byml& item : v)
            self(self, item);

          yaml_sequence_end_event_initialize(&event);
          emitter.Emit(event);
        },
        [&](const Hash& v) {
          const auto style = byml::ShouldUseInlineYamlStyle(v) ? YAML_FLOW_MAPPING_STYLE :
                                                                 YAML_BLOCK_MAPPING_STYLE;
          yml::LibyamlEmitter::MappingScope scope{emitter, {}, style};

          for (const auto& [k, v] : v) {
            emitter.EmitString(k);
            self(self, v);
          }
        },
        [&](bool v) { emitter.EmitBool(v); },  //
        [&](S32 v) { emitter.EmitInt(v); },    //
        [&](F32 v) { emitter.EmitFloat(v); },  //
        [&](U32 v) { emitter.EmitScalar(absl::StrFormat("0x%08x", v), false, false, "!u"); },
        [&](S64 v) { emitter.EmitInt(v, "!l"); },   //
        [&](U64 v) { emitter.EmitInt(v, "!ul"); },  //
        [&](F64 v) { emitter.EmitDouble(v, "!f64"); });
  };
  emit(emit, *this);

  yaml_document_end_event_initialize(&event, 1);
  emitter.Emit(event);

  yaml_stream_end_event_initialize(&event);
  emitter.Emit(event);
  return std::move(emitter.GetOutput());
}

}  // namespace oead
