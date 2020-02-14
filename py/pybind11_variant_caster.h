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

#pragma once

#include <memory>
#include <nonstd/visit.h>
#include <type_traits>
#include <variant>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <oead/types.h>
#include <oead/util/type_utils.h>

namespace oead::detail {
template <class T>
struct RemoveUniquePtr {
  using type = T;
};

template <class T, class D>
struct RemoveUniquePtr<std::unique_ptr<T, D>> {
  using type = T;
};
}  // namespace oead::detail

namespace pybind11::detail {
struct oead_caster_visitor {
  return_value_policy policy;
  handle parent;

  using result_type = handle;  // required by boost::variant in C++11

  template <typename U>
  result_type operator()(U& src) const {
    if constexpr (!oead::util::IsUniquePtr<std::decay_t<U>>()) {
      return make_caster<U>::cast(src, policy, parent);
    } else {
      using T = typename std::decay_t<U>::element_type;
      return make_caster<T>::cast(*src, policy, parent);
    }
  }
};

template <typename Variant>
struct oead_variant_caster;

/// Variant of pybind11::detail::variant_caster
/// which supports std::unique_ptr members.
/// Also disables implicit conversions to bool.
template <template <typename...> class V, typename... Ts>
struct oead_variant_caster<V<Ts...>> {
  template <typename T, bool ptr>
  bool do_load(handle src, bool convert) {
    if constexpr (std::is_same<T, bool>())
      convert = false;
    auto caster = make_caster<T>();
    if (caster.load(src, convert)) {
      if constexpr (ptr) {
        this->value = std::make_unique<T>(cast_op<T>(caster));
      } else {
        this->value = cast_op<T>(caster);
      }
      return true;
    }
    return false;
  }

  template <typename U, typename... Us>
  bool load_alternative(handle src, bool convert, type_list<U, Us...>) {
    bool ok;
    if constexpr (oead::util::IsUniquePtr<std::decay_t<U>>())
      ok = do_load<typename std::decay_t<U>::element_type, true>(src, convert);
    else
      ok = do_load<U, false>(src, convert);
    return ok || load_alternative(src, convert, type_list<Us...>{});
  }

  bool load_alternative(handle, bool, type_list<>) { return false; }

  bool load(handle src, bool convert) {
    if (convert && load_alternative(src, false, type_list<Ts...>{}))
      return true;
    return load_alternative(src, convert, type_list<Ts...>{});
  }

  template <typename T>
  static handle cast(T&& src, return_value_policy policy, handle parent) {
    return rollbear::visit(oead_caster_visitor{policy, parent}, std::forward<T>(src));
  }

  using Type = V<Ts...>;
  PYBIND11_TYPE_CASTER(
      Type,
      _("Union[") +
          detail::concat(make_caster<
                         typename oead::detail::RemoveUniquePtr<std::decay_t<Ts>>::type>::name...) +
          _("]"));
};

template <typename T>
struct oead_variant_wrapper_caster {
  using value_conv = make_caster<typename T::Storage>;

  template <typename T_>
  static handle cast(T_&& src, return_value_policy policy, handle parent) {
    return value_conv::cast(src.v, policy, parent);
  }

  bool load(handle src, bool convert) {
    value_conv inner_caster;
    if (!inner_caster.load(src, convert))
      return false;
    value.v = std::move(cast_op<typename T::Storage&&>(std::move(inner_caster)));
    return true;
  }

  PYBIND11_TYPE_CASTER(T, value_conv::name);
};

}  // namespace pybind11::detail
