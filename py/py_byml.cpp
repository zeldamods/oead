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

#include <functional>
#include <map>
#include <nonstd/span.h>
#include <type_traits>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <oead/byml.h>
#include <oead/util/scope_guard.h>
#include "main.h"

OEAD_MAKE_OPAQUE("Array", oead::Byml::Array);
OEAD_MAKE_OPAQUE("Dictionary", oead::Byml::Dictionary);
OEAD_MAKE_OPAQUE("Hash32", oead::Byml::Hash32);
OEAD_MAKE_OPAQUE("Hash64", oead::Byml::Hash64);
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
/// Wraps a callable to avoid expensive conversions to Byml; instead, we detect
/// whether the first parameter is a Byml::Array, Byml::Dictionary, Byml::Hash32, or Byml::hash64
/// and if so we borrow the value and move-construct a temporary Byml in order to avoid copies.
template <typename... Ts, typename Callable>
static auto BorrowByml(Callable&& callable) {
  return [&, callable](py::handle handle, Ts&&... args) {
    Byml obj;
    const auto invoke = [&, callable](auto&& value) {
      obj = std::move(value);
      return std::invoke(callable, obj, std::forward<Ts>(args)...);
    };
    if (py::isinstance<Byml::Array>(handle)) {
      auto& ref = handle.cast<Byml::Array&>();
      util::ScopeGuard guard{[&] { ref = std::move(obj.Get<Byml::Type::Array>()); }};
      return invoke(std::move(ref));
    }
    if (py::isinstance<Byml::Dictionary>(handle)) {
      auto& ref = handle.cast<Byml::Dictionary&>();
      util::ScopeGuard guard{[&] { ref = std::move(obj.Get<Byml::Type::Dictionary>()); }};
      return invoke(std::move(ref));
    }
    if (py::isinstance<Byml::Hash32>(handle)) {
      auto& ref = handle.cast<Byml::Hash32&>();
      util::ScopeGuard guard{[&] { ref = std::move(obj.Get<Byml::Type::Hash32>()); }};
      return invoke(std::move(ref));
    }
    if (py::isinstance<Byml::Hash64>(handle)) {
      auto& ref = handle.cast<Byml::Hash64&>();
      util::ScopeGuard guard{[&] { ref = std::move(obj.Get<Byml::Type::Hash64>()); }};
      return invoke(std::move(ref));
    }
    return invoke(handle.cast<Byml>());
  };
}

void BindByml(py::module& parent) {
  auto m = parent.def_submodule("byml");
  m.def("from_binary", &Byml::FromBinary, "buffer"_a, py::return_value_policy::move,
        ":return: A Hash, Array, or Dictionary.");
  m.def("from_text", &Byml::FromText, "yml_text"_a, py::return_value_policy::move,
        ":return: A Hash, Array, or Dictionary.");
  m.def("to_binary", BorrowByml<bool, int>(&Byml::ToBinary), "data"_a, "big_endian"_a,
        "version"_a = 2);
  m.def("to_text", BorrowByml(&Byml::ToText), "data"_a);

  m.def("get_bool", BorrowByml(&Byml::GetBool), "data"_a);
  m.def("get_double", BorrowByml(&Byml::GetDouble), "data"_a);
  m.def("get_float", BorrowByml(&Byml::GetFloat), "data"_a);
  m.def("get_int", BorrowByml(&Byml::GetInt), "data"_a);
  m.def("get_int64", BorrowByml(&Byml::GetInt64), "data"_a);
  m.def("get_string", BorrowByml(py::overload_cast<>(&Byml::GetString, py::const_)), "data"_a);
  m.def("get_uint", BorrowByml(&Byml::GetUInt), "data"_a);
  m.def("get_uint64", BorrowByml(&Byml::GetUInt64), "data"_a);

  BindVector<Byml::Array>(m, "Array");
  BindMap<Byml::Dictionary>(m, "Dictionary");
  BindMap<Byml::Hash32>(m, "Hash32");
  BindMap<Byml::Hash64>(m, "Hash64");

  py::class_<Byml::BinaryWithAlignment> clas(m, "BinaryWithAlignment");
  clas.def(py::init<>())
      .def(py::init<std::vector<u8>, U32>())
      .def(py::self == py::self)
      .def_readwrite("data", &Byml::BinaryWithAlignment::data)
      .def_readwrite("alignment", &Byml::BinaryWithAlignment::align)
      .def("__copy__",
           [](const Byml::BinaryWithAlignment& f) { return Byml::BinaryWithAlignment(f); })
      .def("__deepcopy__", [](const Byml::BinaryWithAlignment& f, py::dict) {
        return Byml::BinaryWithAlignment(f);
      });
}
}  // namespace oead::bind
