#include <oead/audio/bars.h>

#include "main.h"

namespace oead::bind {
void BindBars(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::bars::Bars>(m, "Bars")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def(py::init<const std::string&>())
    .def("get_files", &audio::bars::Bars::GetFiles)
    .def("get_file", py::overload_cast<int>(&audio::bars::Bars::GetFile, py::const_), "idx"_a)
    .def("get_file", py::overload_cast<const std::string&>(&audio::bars::Bars::GetFile, py::const_), "name"_a)
    .def("add_file", &audio::bars::Bars::AddFile, "meta"_a, "file"_a)
    .def("to_binary", py::overload_cast<>(&audio::bars::Bars::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::bars::Bars::ToBinary, py::const_), "endian"_a)
    .def("meta_to_binary", py::overload_cast<int>(&audio::bars::Bars::MetaToBinary, py::const_))
    .def("meta_to_binary", py::overload_cast<const std::string&>(&audio::bars::Bars::MetaToBinary, py::const_))
    .def("file_to_binary", py::overload_cast<int>(&audio::bars::Bars::FileToBinary, py::const_))
    .def("file_to_binary", py::overload_cast<const std::string&>(&audio::bars::Bars::FileToBinary, py::const_))
    .def_property(
      "version", 
      py::overload_cast<>(&audio::bars::Bars::Version, py::const_),
      py::overload_cast<u16>(&audio::bars::Bars::Version)
    )
    .def_property(
      "endian",
      py::overload_cast<>(&audio::bars::Bars::Endianness, py::const_),
      py::overload_cast<util::Endianness>(&audio::bars::Bars::Endianness)
    );

  py::class_<audio::bars::Bars::FileWithMetadata>(m, "FileWithMetadata")
    .def_readwrite("meta", &audio::bars::Bars::FileWithMetadata::meta)
    .def_readwrite("asset", &audio::bars::Bars::FileWithMetadata::asset);
}
} // namespace oead::bind