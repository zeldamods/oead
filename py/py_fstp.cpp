#include <oead/audio/fstp.h>

#include "main.h"

namespace oead::bind {
void BindFstp(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fstp::Fstp, std::shared_ptr<audio::fstp::Fstp>, audio::IAssetFile>(m, "Fstp")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def("to_binary", py::overload_cast<>(&audio::fstp::Fstp::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::fstp::Fstp::ToBinary, py::const_), "endian"_a)
    .def_property(
      "version",
      py::overload_cast<>(&audio::fstp::Fstp::Version, py::const_),
      py::overload_cast<u32>(&audio::fstp::Fstp::Version)
    )
    .def_property(
      "info",
      py::overload_cast<>(&audio::fstp::Fstp::Info, py::const_),
      py::overload_cast<audio::fstm::InfoBlock>(&audio::fstp::Fstp::Info)
    )
    .def_property(
      "data",
      py::overload_cast<>(&audio::fstp::Fstp::Data, py::const_),
      py::overload_cast<const audio::fstp::PrefetchDataBlock&>(&audio::fstp::Fstp::Data)
    )
    .def_property(
      "endian",
      py::overload_cast<>(&audio::fstp::Fstp::Endianness, py::const_),
      py::overload_cast<util::Endianness>(&audio::fstp::Fstp::Endianness)
    );

  py::class_<audio::fstp::PrefetchData>(m, "PrefetchData")
    .def(py::init<>())
    .def_readwrite("start_frame", &audio::fstp::PrefetchData::start_frame)
    .def_readwrite("prefetch_samples", &audio::fstp::PrefetchData::prefetch_samples);

  py::class_<audio::fstp::PrefetchDataBlock>(m, "PrefetchDataBlock")
    .def(py::init<>())
    .def_readwrite("prefetch_data", &audio::fstp::PrefetchDataBlock::prefetch_data);
}
} // namespace oead::bind