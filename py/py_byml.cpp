/**
 * Copyright (C) 2019 leoetlino <leo@leolam.fr>
 *
 * This file is part of syaz0.
 *
 * syaz0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * syaz0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with syaz0.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>
#include <nonstd/span.h>
#include <type_traits>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <oead/byml.h>
#include "main.h"

OEAD_MAKE_OPAQUE("oead.Byml.Array", oead::Byml::Array);
OEAD_MAKE_OPAQUE("oead.Byml.Hash", oead::Byml::Hash);
OEAD_MAKE_VARIANT_CASTER(oead::Byml::Value);

namespace oead::bind {
void BindByml(py::module& m) {
  py::class_<Byml> clas(m, "Byml");
  py::enum_<Byml::Type>(clas, "Type")
      .value("Null", Byml::Type::Null)
      .value("String", Byml::Type::String)
      .value("Array", Byml::Type::Array)
      .value("Hash", Byml::Type::Hash)
      .value("Bool", Byml::Type::Bool)
      .value("Int", Byml::Type::Int)
      .value("Float", Byml::Type::Float)
      .value("UInt", Byml::Type::UInt)
      .value("Int64", Byml::Type::Int64)
      .value("UInt64", Byml::Type::UInt64)
      .value("Double", Byml::Type::Double);

  clas.def(py::init<>()).def(py::init<Byml::Value>(), "value"_a).def(py::self == py::self);

  clas.def("type", &Byml::GetType)
      .def("__repr__", [](Byml& i) { return "Byml({!r})"_s.format(i.GetVariant()); })
      .def("__str__", [](Byml& i) { return "{!s}"_s.format(i.GetVariant()); });

  clas.def_static(
          "from_binary", [](py::buffer b) { return Byml::FromBinary(PyBufferToSpan(b)); },
          "buffer"_a)
      .def_static(
          "from_text", [](std::string_view str) { return Byml::FromText(str); }, "yml_text"_a)
      .def("to_binary", &Byml::ToBinary, "big_endian"_a, "version"_a = 2)
      .def("to_text", &Byml::ToText);

  clas.def_property(
          "v", [](Byml& i) -> Byml::Value& { return i.GetVariant(); },
          [](Byml& i, Byml::Value& v) { i.GetVariant() = std::move(v); }, "Value")
      .def("get_hash", &Byml::GetHash, py::return_value_policy::reference_internal)
      .def("get_array", &Byml::GetArray, py::return_value_policy::reference_internal)
      .def("get_string", &Byml::GetString)
      .def("get_bool", &Byml::GetBool)
      .def("get_int", &Byml::GetInt)
      .def("get_uint", &Byml::GetUInt)
      .def("get_float", &Byml::GetFloat)
      .def("get_int64", &Byml::GetInt64)
      .def("get_uint64", &Byml::GetUInt64)
      .def("get_double", &Byml::GetDouble);

  BindVector<Byml::Array>(clas, "Array");
  BindMap<Byml::Hash>(clas, "Hash");

  py::implicitly_convertible<py::dict, Byml>();
  py::implicitly_convertible<Byml::Value, Byml>();
}
}  // namespace oead::bind
