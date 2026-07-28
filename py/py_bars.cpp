#include <oead/audio/bars.h>

#include "main.h"

namespace oead::bind {
void BindBars(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::bars::Bars>(m, "Bars")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def("get_files", &audio::bars::Bars::GetFiles)
    .def("get_file", py::overload_cast<int>(&audio::bars::Bars::GetFile, py::const_), "idx"_a)
    .def("get_file", py::overload_cast<std::string>(&audio::bars::Bars::GetFile, py::const_), "name"_a)
    .def("add_file", &audio::bars::Bars::AddFile, "meta"_a, "file"_a)
    .def("to_binary", py::overload_cast<>(&audio::bars::Bars::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::bars::Bars::ToBinary, py::const_), "endian"_a)
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
    .def_readwrite("metadata", &audio::bars::Bars::FileWithMetadata::metadata)
    .def_readwrite("asset", &audio::bars::Bars::FileWithMetadata::asset);
}
} // namespace oead::bind