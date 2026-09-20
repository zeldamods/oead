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

#include <memory>
#include <optional>

#include <pybind11/typing.h>

#include <oead/sarc.h>
#include "main.h"

OEAD_MAKE_OPAQUE("oead.SarcWriter.FileMap", oead::SarcWriter::FileMap);

namespace oead::bind {

void BindSarc(py::module& m) {
  py::class_<Sarc, std::shared_ptr<Sarc>> cl(m, "Sarc");
  py::class_<Sarc::File> file_cl(m, "File");

  cl.def(py::init([](py::buffer data) {
           // Sarc reads from the buffer for its whole lifetime, so it has to own the export;
           // keeping the Python object alive would not stop it from being resized or closed.
           struct Storage {
             py::buffer_info buffer;
             std::optional<Sarc> sarc;
           };
           auto storage = std::make_shared<Storage>();
           tcb::span<const u8> span;
           if (!RequestSpan(data, storage->buffer, span))
             throw py::type_error("data must be a contiguous bytes-like object");
           storage->sarc.emplace(span);
           return std::shared_ptr<Sarc>(storage, &*storage->sarc);
         }),
         "data"_a)
      .def(py::self == py::self)
      .def("are_files_equal", &Sarc::AreFilesEqual)
      .def("get_num_files", &Sarc::GetNumFiles)
      .def("get_data_offset", &Sarc::GetDataOffset)
      .def("get_endianness", &Sarc::GetEndianness)
      .def("get_file", py::overload_cast<std::string_view>(&Sarc::GetFile, py::const_), "name"_a,
           py::keep_alive<0, 1>())
      .def("get_file", py::overload_cast<u16>(&Sarc::GetFile, py::const_), "index"_a,
           py::keep_alive<0, 1>())
      .def("get_files",
           [](const Sarc& s) -> py::typing::Iterator<Sarc::File> {
             // Going through get_file makes every File keep the archive alive, which values
             // yielded by py::make_iterator do not.
             const auto self = py::cast(&s, py::return_value_policy::reference);
             const auto builtins = py::module_::import("builtins");
             const auto num_files = self.attr("get_num_files")();
             return builtins.attr("map")(self.attr("get_file"), builtins.attr("range")(num_files));
           })
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
      .def(py::init<util::Endianness, SarcWriter::Mode>(), py::arg_v("endian", util::Endianness::Little, "oead.Endianness.Little"),
           py::arg_v("mode", SarcWriter::Mode::New, "oead.SarcWriter.Mode.New"))
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
