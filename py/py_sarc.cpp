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

#include <oead/sarc.h>
#include "main.h"

OEAD_MAKE_OPAQUE("FileMap", oead::SarcWriter::FileMap);

namespace oead::bind {

void BindSarc(py::module& m) {
  py::class_<Sarc> cl(m, "Sarc");
  py::class_<Sarc::File> file_cl(m, "File");

  cl.def(py::init<tcb::span<const u8>>(), "data"_a, py::keep_alive<1, 2>())
      .def("get_num_files", &Sarc::GetNumFiles)
      .def("get_data_offset", &Sarc::GetDataOffset)
      .def("get_endianness", &Sarc::GetEndianness)
      .def("get_file", py::overload_cast<std::string_view>(&Sarc::GetFile, py::const_), "name"_a,
           py::keep_alive<0, 1>())
      .def("get_file", py::overload_cast<u16>(&Sarc::GetFile, py::const_), "index"_a,
           py::keep_alive<0, 1>())
      .def(
          "get_files",
          [](const Sarc& s) { return py::make_iterator(s.GetFiles().begin(), s.GetFiles().end()); },
          py::keep_alive<0, 1>())
      .def("guess_min_alignment", &Sarc::GuessMinAlignment);

  file_cl.def_readonly("name", &Sarc::File::name)
      .def_readonly("data", &Sarc::File::data)
      .def("__repr__", [](const Sarc::File& file) { return "Sarc.File({})"_s.format(file.name); })
      .def("__str__", [](const Sarc::File& file) { return file.name; });

  py::class_<SarcWriter> writer_cl(m, "SarcWriter");

  py::enum_<SarcWriter::Mode>(writer_cl, "Mode")
      .value("Legacy", SarcWriter::Mode::Legacy)
      .value("New", SarcWriter::Mode::New);

  BindMap<SarcWriter::FileMap>(writer_cl, "FileMap");

  writer_cl
      .def(py::init<util::Endianness, SarcWriter::Mode>(), "endian"_a = util::Endianness::Little,
           "mode"_a = SarcWriter::Mode::New)
      .def("write", &SarcWriter::Write, py::return_value_policy::move)
      .def("set_endianness", &SarcWriter::SetEndianness, "endian"_a)
      .def("set_min_alignment", &SarcWriter::SetMinAlignment, "alignment"_a)
      .def("add_alignment_requirement", &SarcWriter::AddAlignmentRequirement,
           "extension_without_dot"_a, "alignment"_a)
      .def("set_mode", &SarcWriter::SetMode, "mode"_a)
      .def_readwrite("files", &SarcWriter::m_files)
      .def_static("from_sarc", &SarcWriter::FromSarc, "archive"_a, py::return_value_policy::move);
}

}  // namespace oead::bind
