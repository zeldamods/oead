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

#include <oead/aamp.h>
#include "main.h"

OEAD_MAKE_VARIANT_CASTER(oead::aamp::Parameter::Value);
OEAD_MAKE_OPAQUE("aamp.ParameterMap", oead::aamp::ParameterMap);
OEAD_MAKE_OPAQUE("aamp.ParameterObjectMap", oead::aamp::ParameterObjectMap);
OEAD_MAKE_OPAQUE("aamp.ParameterListMap", oead::aamp::ParameterListMap);

namespace oead::bind {
static void BindAampParameter(py::module& m) {
  py::class_<aamp::Parameter> clas(m, "Parameter");
  py::enum_<aamp::Parameter::Type>(clas, "Type")
      .value("Bool", aamp::Parameter::Type::Bool)
      .value("F32", aamp::Parameter::Type::F32)
      .value("Int", aamp::Parameter::Type::Int)
      .value("Vec2", aamp::Parameter::Type::Vec2)
      .value("Vec3", aamp::Parameter::Type::Vec3)
      .value("Vec4", aamp::Parameter::Type::Vec4)
      .value("Color", aamp::Parameter::Type::Color)
      .value("String32", aamp::Parameter::Type::String32)
      .value("String64", aamp::Parameter::Type::String64)
      .value("Curve1", aamp::Parameter::Type::Curve1)
      .value("Curve2", aamp::Parameter::Type::Curve2)
      .value("Curve3", aamp::Parameter::Type::Curve3)
      .value("Curve4", aamp::Parameter::Type::Curve4)
      .value("BufferInt", aamp::Parameter::Type::BufferInt)
      .value("BufferF32", aamp::Parameter::Type::BufferF32)
      .value("String256", aamp::Parameter::Type::String256)
      .value("Quat", aamp::Parameter::Type::Quat)
      .value("U32", aamp::Parameter::Type::U32)
      .value("BufferU32", aamp::Parameter::Type::BufferU32)
      .value("BufferBinary", aamp::Parameter::Type::BufferBinary)
      .value("StringRef", aamp::Parameter::Type::StringRef);

  clas.def(py::init<aamp::Parameter::Value>()).def(py::self == py::self);

  clas.def("type", &aamp::Parameter::GetType)
      .def("__repr__",
           [](aamp::Parameter& i) { return "aamp.Parameter({!r})"_s.format(i.GetVariant()); })
      .def("__str__", [](aamp::Parameter& i) { return "{!s}"_s.format(i.GetVariant()); });

  clas.def_property(
      "v", [](aamp::Parameter& i) -> aamp::Parameter::Value& { return i.GetVariant(); },
      [](aamp::Parameter& i, aamp::Parameter::Value& v) { i.GetVariant() = std::move(v); },
      "Value");

  py::implicitly_convertible<aamp::Parameter::Value, aamp::Parameter>();
}

void BindAamp(py::module& parent) {
  py::module m = parent.def_submodule("aamp");

  py::class_<aamp::Name>(m, "Name")
      .def(py::init<u32>(), "name_crc32"_a)
      .def(py::init<std::string_view>(), "name"_a)
      .def(py::self == py::self)
      .def_readonly("hash", &aamp::Name::hash)
      .def("__str__", [](aamp::Name n) { return "{}"_s.format(n.hash); })
      .def("__repr__", [](aamp::Name n) { return "aamp.Name({})"_s.format(n.hash); });
  py::implicitly_convertible<u32, aamp::Name>();
  py::implicitly_convertible<std::string_view, aamp::Name>();

  BindAampParameter(m);

  py::class_<aamp::ParameterObject>(m, "ParameterObject")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("params", &aamp::ParameterObject::params);

  py::class_<aamp::ParameterList>(m, "ParameterList")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("objects", &aamp::ParameterList::objects)
      .def_readwrite("lists", &aamp::ParameterList::lists);

  py::class_<aamp::ParameterIO, aamp::ParameterList>(m, "ParameterIO")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("version", &aamp::ParameterIO::version)
      .def_readwrite("type", &aamp::ParameterIO::type)
      .def_static(
          "from_binary",
          [](py::buffer b) { return aamp::ParameterIO::FromBinary(PyBufferToSpan(b)); }, "buffer"_a)
      .def_static(
          "from_text", [](std::string_view str) { return aamp::ParameterIO::FromText(str); },
          "yml_text"_a)
      .def("to_binary", &aamp::ParameterIO::ToBinary)
      .def("to_text", &aamp::ParameterIO::ToText);

  BindMap<aamp::ParameterMap>(m, "ParameterMap");
  BindMap<aamp::ParameterObjectMap>(m, "ParameterObjectMap");
  BindMap<aamp::ParameterListMap>(m, "ParameterListMap");
}
}  // namespace oead::bind
