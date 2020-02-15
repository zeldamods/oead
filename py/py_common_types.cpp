/**
 * Copyright (C) 2020 leoetlino <leo@leolam.fr>
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

#include <type_traits>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <oead/errors.h>
#include <oead/types.h>
#include "main.h"

namespace oead::bind {
namespace detail {
template <typename T, typename PyT>
void BindNumber(py::module& m, const char* name) {
  py::class_<T> cl(m, name);
  cl.def(py::init([](PyT v) { return T(v); }), "value"_a = 0)
      .def(py::self == py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def_property(
          "v", [](const T& self) { return self.value; }, [](T& self, PyT vnew) { self = vnew; },
          "Value")
      .def("__int__",
           [](const T& self) {
             if constexpr (std::is_floating_point<decltype(T::value)>())
               return s64(self.value);
             else
               return self.value;
           })
      .def("__float__", [](const T& self) { return double(self.value); })
      .def("__str__", [](const T& self) { return std::to_string(self.value); })
      .def("__repr__", [name](const T& self) { return "{}({})"_s.format(name, self.value); });

  if constexpr (std::is_integral<decltype(T::value)>()) {
    cl.def("__index__", [](const T& self) { return self.value; });
  }
}

template <size_t N>
void BindFixedSafeString(py::module& m, const char* name) {
  py::class_<FixedSafeString<N>>(m, name)
      .def(py::init<>())
      .def(py::init<std::string_view>())
      .def("__str__", [](const FixedSafeString<N>& s) { return Str(s); })
      .def("__repr__",
           [name](const FixedSafeString<N>& s) { return "{}({})"_s.format(name, Str(s)); })
      .def(py::self == py::self)
      .def("__eq__",
           [](const FixedSafeString<N>& lhs, std::string_view rhs) { return Str(lhs) == rhs; });
}
}  // namespace detail

void BindCommonTypes(py::module& m) {
  BindVector<std::vector<u8>>(
      m, "Bytes", py::buffer_protocol(),
      "Mutable bytes-like object. This is used to avoid possibly expensive data copies.");
  BindVector<std::vector<int>>(m, "BufferInt", py::buffer_protocol(),
                               "Mutable list-like object that stores signed 32-bit integers.");
  BindVector<std::vector<f32>>(m, "BufferF32", py::buffer_protocol(),
                               "Mutable list-like object that stores binary32 floats.");
  BindVector<std::vector<u32>>(m, "BufferU32", py::buffer_protocol(),
                               "Mutable list-like object that stores unsigned 32-bit integers.");

  detail::BindNumber<U8, py::int_>(m, "U8");
  detail::BindNumber<U16, py::int_>(m, "U16");
  detail::BindNumber<U32, py::int_>(m, "U32");
  detail::BindNumber<U64, py::int_>(m, "U64");

  detail::BindNumber<S8, py::int_>(m, "S8");
  detail::BindNumber<S16, py::int_>(m, "S16");
  detail::BindNumber<S32, py::int_>(m, "S32");
  detail::BindNumber<S64, py::int_>(m, "S64");

  detail::BindNumber<F32, py::float_>(m, "F32");
  detail::BindNumber<F64, py::float_>(m, "F64");

  py::class_<Vector2f>(m, "Vector2f")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("x", &Vector2f::x)
      .def_readwrite("y", &Vector2f::y);

  py::class_<Vector3f>(m, "Vector3f")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("x", &Vector3f::x)
      .def_readwrite("y", &Vector3f::y)
      .def_readwrite("z", &Vector3f::z);

  py::class_<Vector4f>(m, "Vector4f")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("x", &Vector4f::x)
      .def_readwrite("y", &Vector4f::y)
      .def_readwrite("z", &Vector4f::z)
      .def_readwrite("t", &Vector4f::t);

  py::class_<Quatf>(m, "Quatf")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("a", &Quatf::a)
      .def_readwrite("b", &Quatf::b)
      .def_readwrite("c", &Quatf::c)
      .def_readwrite("d", &Quatf::d);

  py::class_<Color4f>(m, "Color4f")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("r", &Color4f::r)
      .def_readwrite("g", &Color4f::g)
      .def_readwrite("b", &Color4f::b)
      .def_readwrite("a", &Color4f::a);

  py::class_<Curve>(m, "Curve")
      .def(py::init<>())
      .def(py::self == py::self)
      .def_readwrite("a", &Curve::a)
      .def_readwrite("b", &Curve::b)
      .def_readwrite("floats", &Curve::floats);

  detail::BindFixedSafeString<16>(m, "FixedSafeString16");
  detail::BindFixedSafeString<32>(m, "FixedSafeString32");
  detail::BindFixedSafeString<48>(m, "FixedSafeString48");
  detail::BindFixedSafeString<64>(m, "FixedSafeString64");
  detail::BindFixedSafeString<128>(m, "FixedSafeString128");
  detail::BindFixedSafeString<256>(m, "FixedSafeString256");

  py::register_exception<TypeError>(m, "TypeError");
  py::register_exception<InvalidDataError>(m, "InvalidDataError");
}
}  // namespace oead::bind
