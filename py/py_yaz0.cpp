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

#include <nonstd/span.h>
#include <vector>

#include <oead/errors.h>
#include <oead/types.h>
#include <oead/yaz0.h>
#include "main.h"

namespace oead::bind {

void BindYaz0(py::module& parent) {
  auto m = parent.def_submodule("yaz0");

  py::class_<yaz0::Header>(m, "Header")
      .def_readwrite("magic", &yaz0::Header::magic)
      .def_readwrite("uncompressed_size", &yaz0::Header::uncompressed_size)
      .def_readwrite("data_alignment", &yaz0::Header::data_alignment)
      .def_readwrite("reserved", &yaz0::Header::reserved);

  m.def("get_header", &yaz0::GetHeader, "data"_a);

  m.def(
      "decompress",
      [](tcb::span<const u8> src) {
        const auto header = yaz0::GetHeader(src);
        if (!header)
          throw InvalidDataError("Invalid Yaz0 header");
        py::bytes dst_py{nullptr, header->uncompressed_size};
        yaz0::Decompress(src, PyBytesToSpan(dst_py));
        return dst_py;
      },
      "data"_a);

  m.def(
      "decompress_unsafe",
      [](tcb::span<const u8> src) {
        py::bytes dst_py{nullptr, yaz0::GetHeader(src)->uncompressed_size};
        yaz0::DecompressUnsafe(src, PyBytesToSpan(dst_py));
        return dst_py;
      },
      "data"_a);

  m.def("compress", &yaz0::Compress, "data"_a, "data_alignment"_a = 0, "level"_a = 7);
}

}  // namespace oead::bind
