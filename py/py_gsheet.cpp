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

#include <oead/gsheet.h>
#include <vector>
#include "main.h"

OEAD_MAKE_OPAQUE("oead.gsheet.FieldArray", std::vector<oead::gsheet::Field>)
OEAD_MAKE_OPAQUE("oead.gsheet.Struct", oead::gsheet::Data::Struct)
OEAD_MAKE_OPAQUE("oead.gsheet.StructArray", std::vector<oead::gsheet::Data::Struct>)

OEAD_MAKE_VARIANT_CASTER(oead::gsheet::Data::Variant)

namespace pybind11::detail {
struct oead_gsheet_data_caster {
  using value_conv = make_caster<oead::gsheet::Data::Variant>;

  template <typename T_>
  static handle cast(T_&& src, return_value_policy policy, handle parent) {
    return value_conv::cast(src.v, policy, parent);
  }

  bool load(handle src, bool convert) {
    value_conv inner_caster;
    if (!inner_caster.load(src, convert))
      return false;
    value.v = std::move(cast_op<oead::gsheet::Data::Variant&&>(std::move(inner_caster)));
    return true;
  }

  PYBIND11_TYPE_CASTER(oead::gsheet::Data, value_conv::name);
};

template <>
struct type_caster<oead::gsheet::Data> : oead_gsheet_data_caster {};
}  // namespace pybind11::detail

namespace oead::bind {

void BindGsheet(py::module& parent) {
  py::module m = parent.def_submodule("gsheet");

  auto Field = py::class_<gsheet::Field>(m, "Field", "Grezzo datasheet field.");
  BindVector<std::vector<gsheet::Field>>(m, "FieldArray");

  py::enum_<gsheet::Field::Type>(Field, "Type")
      .value("Struct", gsheet::Field::Type::Struct, "C/C++ style structure.")
      .value("Bool", gsheet::Field::Type::Bool, "Boolean.")
      .value("Int", gsheet::Field::Type::Int, "Signed 32-bit integer.")
      .value("Float", gsheet::Field::Type::Float,
             "Single-precision floating point number (binary32)..")
      .value("String", gsheet::Field::Type::String, "Null-terminated string.");

  py::enum_<gsheet::Field::Flag>(Field, "Flag")
      .value("IsNullable", gsheet::Field::Flag::IsNullable)
      .value("IsArray", gsheet::Field::Flag::IsArray)
      .value("IsKey", gsheet::Field::Flag::IsKey)
      .value("Unknown3", gsheet::Field::Flag::Unknown3)
      .value("IsEnum", gsheet::Field::Flag::IsEnum)
      .value("Unknown5", gsheet::Field::Flag::Unknown5);

  Field.def(py::init<>())
      .def_readwrite("name", &gsheet::Field::name, "Name (must not be empty).")
      .def_readwrite("type_name", &gsheet::Field::type_name, "Type name.")
      .def_readwrite("type", &gsheet::Field::type, "Field type.")
      .def_readwrite("x11", &gsheet::Field::x11, "Unknown; depth level?")
      .def_property(
          "flags", [](gsheet::Field& self) { return self.flags.m_hex; },
          [](gsheet::Field& self, u32 flags) { self.flags.m_hex = flags; }, "Flags.")
      .def_readwrite("offset_in_value", &gsheet::Field::offset_in_value,
                     "Offset of this field in the value structure.")
      .def_readwrite(
          "inline_size", &gsheet::Field::inline_size,
          "Size of this field in the value structure. For strings and arrays, this is always 0x10.")
      .def_readwrite("data_size", &gsheet::Field::data_size,
                     "Size of the field data. For strings and inline types (inline structs, ints, "
                     "floats, bools,), this is the same as the inline size.")
      .def_readwrite("fields", &gsheet::Field::fields, "[For structs] Fields")
      .def("__repr__", [](const gsheet::Field& self) {
        return "<Field: {} {}>"_s.format(self.type_name, self.name);
      });

  BindMap<gsheet::Data::Struct>(m, "Struct",
                                "Grezzo datasheet struct. In this API, a Struct is represented as "
                                "a dict-like object, with the field names as keys.");

  BindVector<std::vector<gsheet::Data::Struct>>(m, "StructArray", "A list of Struct elements.");
  m.attr("BoolArray") = parent.attr("BufferBool");
  m.attr("IntArray") = parent.attr("BufferInt");
  m.attr("FloatArray") = parent.attr("BufferF32");
  m.attr("StringArray") = parent.attr("BufferString");

  auto Sheet = py::class_<gsheet::SheetRw>(m, "Sheet", "Grezzo datasheet.");

  Sheet.def(py::init<>())
      .def_readwrite("alignment", &gsheet::SheetRw::alignment)
      .def_readwrite("hash", &gsheet::SheetRw::hash)
      .def_readwrite("name", &gsheet::SheetRw::name)
      .def_readwrite("root_fields", &gsheet::SheetRw::root_fields)
      .def_readwrite("values", &gsheet::SheetRw::values)
      .def("__repr__",
           [](const gsheet::SheetRw& self) { return "<Datasheet: {}>"_s.format(self.name); })
      .def("to_binary", &gsheet::SheetRw::ToBinary, "Convert the sheet to a binary datasheet.");

  m.def(
      "parse", [](std::vector<u8> data) { return gsheet::Sheet{data}.MakeRw(); }, "data"_a,
      "Parse a binary datasheet.");

  m.def(
      "test_roundtrip",
      [](std::vector<u8> data) {
        gsheet::Sheet sheet{data};
        return sheet.MakeRw().ToBinary();
      },
      "data"_a, "Parse a binary datasheet and immediately dump it back for testing purposes.");
}

}  // namespace oead::bind
