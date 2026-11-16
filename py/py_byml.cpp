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
#include <span>
#include <type_traits>
#include <vector>

#include <pybind11/native_enum.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <oead/byml.h>
#include <oead/util/scope_guard.h>
#include "main.h"

OEAD_MAKE_OPAQUE("oead.byml.Array", oead::Byml::Array);
OEAD_MAKE_OPAQUE("oead.byml.Hash", oead::Byml::Hash);
OEAD_MAKE_OPAQUE("oead.byml.OrderedHash", oead::Byml::OrderedHash);
OEAD_MAKE_OPAQUE("oead.byml.Hash32", oead::Byml::Hash32);
OEAD_MAKE_OPAQUE("oead.byml.Hash64", oead::Byml::Hash64);
OEAD_MAKE_OPAQUE("oead.byml.OrderedHash32", oead::Byml::OrderedHash32);
OEAD_MAKE_OPAQUE("oead.byml.OrderedHash64", oead::Byml::OrderedHash64);
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
/// whether the first parameter is a Byml::Array, Byml::Hash, Byml::Hash32, or Byml::Hash64
/// and if so we borrow the value and move-construct a temporary Byml in order to avoid copies.
template <typename T, Byml::Type type>
struct Borrowable {};

template <typename... Ts, typename Callable>
static auto BorrowByml(Callable&& callable) {
  return [&, callable](py::handle handle, Ts&&... args) {
    Byml obj;
    const auto invoke = [&] { return std::invoke(callable, obj, std::forward<Ts>(args)...); };

    const auto borrow_and_invoke = [&]<typename T, Byml::Type type, typename... Rest>(
                                       const auto& self, Borrowable<T, type>, Rest... rest) {
      if (!py::isinstance<T>(handle)) {
        if constexpr (sizeof...(Rest) == 0) {
          obj = handle.cast<Byml>();
          return invoke();
        } else {
          return self(self, rest...);
        }
      }

      auto& ref = handle.cast<T&>();
      obj = std::move(ref);
      util::ScopeGuard guard{[&] { ref = std::move(obj.Get<type>()); }};
      return invoke();
    };

    return borrow_and_invoke(borrow_and_invoke, Borrowable<Byml::Array, Byml::Type::Array>{},
                             Borrowable<Byml::MonoTypedArray, Byml::Type::MonoTypedArray>{},
                             Borrowable<Byml::Hash, Byml::Type::Hash>{},
                             Borrowable<Byml::OrderedHash, Byml::Type::OrderedHash>{},
                             Borrowable<Byml::Hash32, Byml::Type::Hash32>{},
                             Borrowable<Byml::Hash64, Byml::Type::Hash64>{},
                             Borrowable<Byml::OrderedHash32, Byml::Type::OrderedHash32>{},
                             Borrowable<Byml::OrderedHash64, Byml::Type::OrderedHash64>{});
  };
}

void BindByml(py::module& parent) {
  auto m = parent.def_submodule("byml");

  py::native_enum<Byml::Type>(m, "Type", "enum.Enum")
      .value("Null", Byml::Type::Null)
      .value("Hash32", Byml::Type::Hash32)
      .value("Hash64", Byml::Type::Hash64)
      .value("OrderedHash32", Byml::Type::OrderedHash32)
      .value("OrderedHash64", Byml::Type::OrderedHash64)
      .value("String", Byml::Type::String)
      .value("Binary", Byml::Type::Binary)
      .value("AlignedBinary", Byml::Type::AlignedBinary)
      .value("Array", Byml::Type::Array)
      .value("MonoTypedArray", Byml::Type::MonoTypedArray)
      .value("Hash", Byml::Type::Hash)
      .value("OrderedHash", Byml::Type::OrderedHash)
      .value("Bool", Byml::Type::Bool)
      .value("Int", Byml::Type::Int)
      .value("Float", Byml::Type::Float)
      .value("UInt", Byml::Type::UInt)
      .value("Int64", Byml::Type::Int64)
      .value("UInt64", Byml::Type::UInt64)
      .value("Double", Byml::Type::Double)
      .finalize();

  py::class_<Byml::AlignedBinary>(m, "AlignedBinary")
      .def(py::init<>())
      .def(py::init<std::vector<u8>, U32>())
      .def(py::self == py::self)
      .def_readwrite("data", &Byml::AlignedBinary::data)
      .def_readwrite("alignment", &Byml::AlignedBinary::align)
      .def("__copy__", [](const Byml::AlignedBinary& f) { return Byml::AlignedBinary(f); })
      .def("__deepcopy__",
           [](const Byml::AlignedBinary& f, py::dict) { return Byml::AlignedBinary(f); });

  py::class_<Byml::MonoTypedArray>(m, "MonoTypedArray")
      .def(py::init<>())
      .def(py::init<Byml::Type>())
      .def(py::init<Byml::Array, Byml::Type>())
      .def(py::self == py::self)
      .def_readwrite("elements", &Byml::MonoTypedArray::elements)
      .def_readwrite("type", &Byml::MonoTypedArray::type)
      .def("__copy__", [](const Byml::MonoTypedArray& f) { return Byml::MonoTypedArray(f); })
      .def("__deepcopy__",
           [](const Byml::MonoTypedArray& f, py::dict) { return Byml::MonoTypedArray(f); });

  m.def("from_binary", &Byml::FromBinary, "buffer"_a, py::return_value_policy::move,
        ":return: An Array or Hash (value type is possible on versions >= 8).");
  m.def("from_text", &Byml::FromText, "yml_text"_a, py::return_value_policy::move,
        ":return: An Array or Hash (value type is possible on versions >= 8).");
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
  BindMap<Byml::Hash>(m, "Hash");
  BindMap<Byml::OrderedHash>(m, "OrderedHash");
  BindMap<Byml::Hash32>(m, "Hash32");
  BindMap<Byml::Hash64>(m, "Hash64");
  BindMap<Byml::OrderedHash32>(m, "OrderedHash32");
  BindMap<Byml::OrderedHash64>(m, "OrderedHash64");
}
}  // namespace oead::bind
