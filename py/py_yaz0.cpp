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

  auto header_cl = py::class_<yaz0::Header>(m, "Header");
  header_cl.def_readwrite("magic", &yaz0::Header::magic);
  BindEndianInt(header_cl, "uncompressed_size", &yaz0::Header::uncompressed_size);
  BindEndianInt(header_cl, "data_alignment", &yaz0::Header::data_alignment);
  header_cl.def_readwrite("reserved", &yaz0::Header::reserved);

  m.def(
      "get_header",
      [](py::buffer data_py) { return yaz0::GetHeader(PyBufferToSpan(data_py)).value(); },
      "data"_a);

  m.def(
      "decompress",
      [](py::buffer src_py) {
        const auto src = PyBufferToSpan(src_py);
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
      [](py::bytes src_py) {
        const auto src = PyBytesToSpan(src_py);
        py::bytes dst_py{nullptr, yaz0::GetHeader(src)->uncompressed_size};
        yaz0::DecompressUnsafe(src, PyBytesToSpan(dst_py));
        return dst_py;
      },
      "data"_a);

  m.def(
      "compress",
      [](py::buffer src_py, u32 data_alignment, int level) {
        const auto src = PyBufferToSpan(src_py);
        return yaz0::Compress(src, data_alignment, level);
      },
      "data"_a, "data_alignment"_a = 0, "level"_a = 7);
}

}  // namespace oead::bind
