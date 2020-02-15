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

OEAD_MAKE_OPAQUE("Array", oead::Byml::Array);
OEAD_MAKE_OPAQUE("Hash", oead::Byml::Hash);
OEAD_MAKE_VARIANT_CASTER(oead::Byml::Value);

namespace pybind11::detail {
template <>
struct type_caster<oead::Byml> {
  using value_conv = make_caster<oead::Byml::Value>;

  template <typename T_>
  static handle cast(T_&& src, return_value_policy policy, handle parent) {
    return value_conv::cast(src.GetVariant(), policy, parent);
  }

  bool load(handle src, bool convert) {
    value_conv inner_caster;
    if (!inner_caster.load(src, convert))
      return false;
    value.GetVariant() = std::move(cast_op<oead::Byml::Value&&>(std::move(inner_caster)));
    return true;
  }

  PYBIND11_TYPE_CASTER(oead::Byml, make_caster<oead::Byml::Value::Storage>::name);
};
}  // namespace pybind11::detail

namespace oead::bind {
void BindByml(py::module& parent) {
  auto m = parent.def_submodule("byml");
  m.def(
      "from_binary", [](py::buffer b) { return Byml::FromBinary(PyBufferToSpan(b)); }, "buffer"_a,
      py::return_value_policy::move, ":return: An Array or a Hash.");
  m.def("from_text", &Byml::FromText, "yml_text"_a, py::return_value_policy::move,
        ":return: An Array or a Hash.");
  m.def("to_binary", &Byml::ToBinary, "data"_a, "big_endian"_a, "version"_a = 2);
  m.def("to_text", &Byml::ToText, "data"_a);

  m.def("get_bool", &Byml::GetBool, "data"_a);
  m.def("get_double", &Byml::GetDouble, "data"_a);
  m.def("get_float", &Byml::GetFloat, "data"_a);
  m.def("get_int", &Byml::GetInt, "data"_a);
  m.def("get_int64", &Byml::GetInt64, "data"_a);
  m.def("get_string", &Byml::GetString, "data"_a);
  m.def("get_uint", &Byml::GetUInt, "data"_a);
  m.def("get_uint64", &Byml::GetUInt64, "data"_a);

  BindVector<Byml::Array>(m, "Array");
  BindMap<Byml::Hash>(m, "Hash");
}
}  // namespace oead::bind
