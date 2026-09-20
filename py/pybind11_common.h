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

// To be included in every translation unit.

#pragma once

#include <nonstd/span.h>
#include <optional>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <oead/types.h>
#include <oead/util/scope_guard.h>
#include "pybind11_variant_caster.h"

namespace py = pybind11;
using namespace py::literals;

namespace pybind11::detail {
// pybind11 3.1 runs postcall hooks even when the arguments failed to load, in which case the
// return value is the "try next overload" sentinel and keep_alive<0, N> dereferences it.
// This has to be declared before anything instantiates the primary template, including
// bind_map and bind_vector, which use keep_alive<0, 1> themselves.
template <>
struct process_attribute<keep_alive<0, 1>> : process_attribute_default<keep_alive<0, 1>> {
  static void postcall(function_call& call, handle ret) {
    if (ret.ptr() != PYBIND11_TRY_NEXT_OVERLOAD)
      keep_alive_impl(0, 1, call, ret);
  }
};
}  // namespace pybind11::detail

#define OEAD_MAKE_OPAQUE(NAME, ...)                                                                \
  PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)                                                     \
  namespace detail {                                                                               \
  template <>                                                                                      \
  class type_caster<__VA_ARGS__> : public type_caster_base<__VA_ARGS__> {                          \
  public:                                                                                          \
    static constexpr auto name = _(NAME);                                                          \
  };                                                                                               \
  }                                                                                                \
  PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)

#define OEAD_MAKE_VARIANT_CASTER(...)                                                              \
  PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)                                                     \
  namespace detail {                                                                               \
  template <>                                                                                      \
  struct type_caster<__VA_ARGS__::Storage> : oead_variant_caster<__VA_ARGS__::Storage> {};         \
  template <>                                                                                      \
  struct type_caster<__VA_ARGS__> : oead_variant_wrapper_caster<__VA_ARGS__> {};                   \
  }                                                                                                \
  PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)

namespace oead::bind {
/// Exposes memory that belongs to another Python object as a buffer and keeps that object alive.
/// A plain memoryview over raw memory has no owner and dangles once the owner is collected.
struct SpanView {
  py::object owner;
  const void* data;
  size_t size_bytes;
  bool readonly;
};

/// Exports src as a contiguous 1-D buffer of T. The span is only valid while info is alive.
template <typename T>
bool RequestSpan(py::handle src, py::buffer_info& info, tcb::span<T>& span) {
  if (!PyObject_CheckBuffer(src.ptr()))
    return false;
  try {
    info = py::reinterpret_borrow<py::buffer>(src).request(!std::is_const_v<T>);
  } catch (const py::error_already_set&) {
    return false;
  }
  if (info.itemsize != sizeof(T) || info.ndim != 1)
    return false;
  if (info.size > 1 && info.strides[0] != info.itemsize)
    return false;
  span = {static_cast<T*>(info.ptr), size_t(info.size)};
  return true;
}
}  // namespace oead::bind

namespace pybind11::detail {
template <typename T, typename std::enable_if_t<std::is_same_v<std::decay_t<T>, u8>, bool> = true>
constexpr auto OeadGetSpanCasterName() {
  return _("collections.abc.Buffer");
}

template <typename T, typename std::enable_if_t<!std::is_same_v<std::decay_t<T>, u8>, bool> = true>
constexpr auto OeadGetSpanCasterName() {
  return _("Span[") + detail::concat(make_caster<T>::name) + _("]");
}

template <typename T>
struct type_caster<tcb::span<T>> {
  static handle cast(tcb::span<T> span, return_value_policy, handle parent) {
    oead::bind::SpanView view{reinterpret_borrow<object>(parent), span.data(), span.size_bytes(),
                              std::is_const_v<T>};
    return py::memoryview(py::cast(std::move(view))).release();
  }

  bool load(handle src, bool) {
    return oead::bind::RequestSpan(src, m_buffer, value);
  }

private:
  // The export has to outlive the call: releasing it would let the owner resize or free
  // the memory that the span points to.
  py::buffer_info m_buffer;

public:
  PYBIND11_TYPE_CASTER(tcb::span<T>, OeadGetSpanCasterName<T>());
};
}  // namespace pybind11::detail

namespace oead::bind {
inline tcb::span<u8> PyBytesToSpan(py::bytes b) {
  return {reinterpret_cast<u8*>(PYBIND11_BYTES_AS_STRING(b.ptr())),
          size_t(PYBIND11_BYTES_SIZE(b.ptr()))};
}

template <typename Vector, typename holder_type = std::unique_ptr<Vector>, typename... Args>
py::class_<Vector, holder_type> BindVector(py::handle scope, const std::string& name,
                                           Args&&... args) {
  using Value = typename Vector::value_type;
  auto cl = py::bind_vector<Vector, holder_type>(scope, name, std::forward<Args>(args)...);
  // bind_vector's __repr__ streams elements with operator<< (u8 as raw chars, unquoted strings).
  // It has to be removed first; otherwise ours would only be added as a never-reached overload.
  if (cl.attr("__dict__").contains("__repr__"))
    py::delattr(cl, "__repr__");
  cl.def("__repr__", [name](const Vector& v) {
    if constexpr (std::is_same_v<Value, u8>) {
      return "{}({!r})"_s.format(name, py::bytes(reinterpret_cast<const char*>(v.data()), v.size()));
    } else {
      return "{}({!r})"_s.format(name, py::list(py::cast(v, py::return_value_policy::reference)));
    }
  });
  cl.def(py::self == py::self);
  py::implicitly_convertible<py::list, Vector>();
  return cl;
}

// Registered once at the top level so that every bound map shares the same view types
// regardless of which map happens to be bound first.
inline void BindMapViews(py::handle scope) {
  using KeysView = py::detail::keys_view;
  using ValuesView = py::detail::values_view;
  using ItemsView = py::detail::items_view;

  py::class_<KeysView>(scope, "KeysView", py::module_local())
      .def("__len__", &KeysView::len)
      .def("__iter__", &KeysView::iter, py::keep_alive<0, 1>())
      .def("__contains__", &KeysView::contains);

  py::class_<ValuesView>(scope, "ValuesView", py::module_local())
      .def("__len__", &ValuesView::len)
      .def("__iter__", &ValuesView::iter, py::keep_alive<0, 1>());

  py::class_<ItemsView>(scope, "ItemsView", py::module_local())
      .def("__len__", &ItemsView::len)
      .def("__iter__", &ItemsView::iter, py::keep_alive<0, 1>());
}

template <typename Map, typename Key, typename CastFn>
static Map MapFromIter(py::iterator it, CastFn cast_value) {
  Map map;
  while (it != py::iterator::sentinel()) {
    auto pair = py::cast<std::pair<py::handle, py::handle>>(*it);
    map.emplace(pair.first.cast<Key>(), cast_value(pair.second));
    ++it;
  }
  return map;
}

template <typename Map, typename Key, typename CastFn>
static Map MapFromDict(py::dict dict, CastFn cast_value) {
  Map map;
  for (std::pair<py::handle, py::handle> pair : dict)
    map.emplace(pair.first.cast<Key>(), cast_value(pair.second));
  return map;
}

template <typename Map, typename Key, typename Value>
static Value MapCastValue(py::handle handle) {
  if constexpr (std::is_convertible<Map, Value>()) {
    if (py::isinstance<py::dict>(handle))
      return MapFromDict<Map, Key>(handle.cast<py::dict>(), MapCastValue<Map, Key, Value>);
    if (py::isinstance<py::iterator>(handle))
      return MapFromIter<Map, Key>(handle.cast<py::iterator>(), MapCastValue<Map, Key, Value>);
  }
  return handle.cast<Value>();
}

template <class T, class = void>
struct iterator_has_value_member_fn : std::false_type {};
template <class T>
struct iterator_has_value_member_fn<T, std::void_t<decltype(std::declval<T>().value())>> : std::true_type {};

// If we detect a tsl::ordered_map, use a custom
// assignment algorithm, else just use the one
// provided by pybind
template <typename Map, typename Class_>
void MapAssignment(
    std::enable_if_t<std::is_copy_assignable<typename Map::mapped_type>::value, Class_> &cl) {
    using KeyType = typename Map::key_type;
    using MappedType = typename Map::mapped_type;

    if constexpr (iterator_has_value_member_fn<typename Map::iterator>())
      cl.def("__setitem__", [](Map &m, const KeyType &k, const MappedType &v) {
          m.insert_or_assign(k, v);
      });
    else
      py::detail::map_assignment<Map, Class_>(cl);
}

template <typename Map, typename Class_>
void DefineCustomMap(Class_& cl) {
  using KeyType = typename Map::key_type;
  using MappedType = typename Map::mapped_type;
  
  cl.def(py::init([&](py::iterator it) {
      return MapFromIter<Map, KeyType>(it, MapCastValue<Map, KeyType, MappedType>);
    }),
    "iterator"_a
  );

  cl.def(py::init([&](py::dict dict) {
      return MapFromDict<Map, KeyType>(dict, MapCastValue<Map, KeyType, MappedType>);
    }),
    "dictionary"_a
  );

  cl.def(py::self == py::self);

  cl.def(
    "__contains__",
    [](const Map& map, const py::object& arg) {
      try {
        auto key = py::cast<KeyType>(arg);
        return map.find(key) != map.end();
      } catch (const py::cast_error&) {
        return false;
      }
    },
    py::prepend{}
  );

  cl.def("clear", &Map::clear);

  cl.def(
    "get",
    [](const Map& map, const KeyType& key, py::object default_value) -> py::object {
      if (map.find(key) == map.end())
        return default_value;
      return py::cast(&map).attr("__getitem__")(key);
    },
    "key"_a,
    "default"_a = py::none()
  );

  py::implicitly_convertible<py::dict, Map>();
}

template <typename Map, typename holder_type = std::unique_ptr<Map>, typename... Args, 
          typename std::enable_if_t<!iterator_has_value_member_fn<typename Map::iterator>::value, bool> = true>
py::class_<Map, holder_type> BindMap(py::handle scope, const std::string& name, Args&&... args) {
  auto cl = py::bind_map<Map, holder_type>(scope, name, std::forward<Args>(args)...);
  DefineCustomMap<Map>(cl);
  return cl;
}

// Reimplementation of pybind11::bind_map
// to support tsl::ordered_map
template <typename Map, typename holder_type = std::unique_ptr<Map>, typename... Args, 
          typename std::enable_if_t<iterator_has_value_member_fn<typename Map::iterator>::value, bool> = false>
py::class_<Map, holder_type> BindMap(py::handle scope, const std::string& name, Args&&... args) {
  using KeyType = typename Map::key_type;
  using MappedType = typename Map::mapped_type;
  using KeysView = py::detail::keys_view;
  using ValuesView = py::detail::values_view;
  using ItemsView = py::detail::items_view;
  using Class_ = py::class_<Map, holder_type>;

  auto *tinfo = py::detail::get_type_info(typeid(MappedType));
  bool local = !tinfo || tinfo->module_local;
  if (local) {
      tinfo = py::detail::get_type_info(typeid(KeyType));
      local = !tinfo || tinfo->module_local;
  }

  Class_ cl(scope, name.c_str(), pybind11::module_local(local), std::forward<Args>(args)...);


  cl.def(py::init<>());

  py::detail::map_if_insertion_operator<Map, Class_>(cl, name);

  cl.def(
      "__bool__",
      [](const Map &m) -> bool { return !m.empty(); },
      "Check whether the map is nonempty");

  cl.def(
      "__iter__",
      [](Map &m) { return py::make_key_iterator(m.begin(), m.end()); },
      py::keep_alive<0, 1>()
  );

  cl.def(
      "keys",
      [](Map &m) { return std::unique_ptr<KeysView>(new py::detail::KeysViewImpl<Map>(m)); },
      py::keep_alive<0, 1>()
  );

  cl.def(
      "values",
      [](Map &m) { return std::unique_ptr<ValuesView>(new py::detail::ValuesViewImpl<Map>(m)); },
      py::keep_alive<0, 1>()
  );

  cl.def(
      "items",
      [](Map &m) { return std::unique_ptr<ItemsView>(new py::detail::ItemsViewImpl<Map>(m)); },
      py::keep_alive<0, 1>()
  );

  cl.def(
    "__getitem__",
    [](Map &m, const KeyType &k) -> MappedType & {
      auto it = m.find(k);
      if (it == m.end()) {
          set_error(PyExc_KeyError, py::detail::format_message_key_error(k));
          throw py::error_already_set();
      }
      return it.value();
    },
    py::return_value_policy::reference_internal
  );

  cl.def("__contains__", [](Map &m, const KeyType &k) -> bool {
      auto it = m.find(k);
      if (it == m.end()) {
          return false;
      }
      return true;
  });
 
  cl.def("__contains__", [](Map &, const py::object &) -> bool { return false; });

  MapAssignment<Map, Class_>(cl);

  cl.def("__delitem__", [](Map &m, const KeyType &k) {
    auto it = m.find(k);
    if (it == m.end()) {
        set_error(PyExc_KeyError, py::detail::format_message_key_error(k));
        throw py::error_already_set();
    }
    m.erase(it);
  });


  cl.def("__len__", [](const Map &m) { return m.size(); });

  DefineCustomMap<Map>(cl);
  return cl;
}
}  // namespace oead::bind

OEAD_MAKE_OPAQUE("oead.Bytes", std::vector<u8>);
OEAD_MAKE_OPAQUE("oead.BufferInt", std::vector<int>);
OEAD_MAKE_OPAQUE("oead.BufferF32", std::vector<f32>);
OEAD_MAKE_OPAQUE("oead.BufferU32", std::vector<u32>);
OEAD_MAKE_OPAQUE("oead.BufferBool", std::vector<bool>);
OEAD_MAKE_OPAQUE("oead.BufferString", std::vector<std::string>);
