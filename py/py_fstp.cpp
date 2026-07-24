#include <oead/audio/fstp.h>

#include "main.h"

namespace oead::bind {
void BindFstp(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fstp::Fstp, std::shared_ptr<audio::fstp::Fstp>, audio::IAssetFile>(m, "Fstp")
    .def(py::init<tcb::span<const u8>>())
    .def("info", &audio::fstp::Fstp::Info)
    .def("data", &audio::fstp::Fstp::Data)
    .def("endian", py::overload_cast<util::Endianness>(&audio::fstp::Fstp::Endianness))
    .def("endian", py::overload_cast<>(&audio::fstp::Fstp::Endianness, py::const_))
    .def("to_binary", &audio::fstp::Fstp::ToBinary);

  py::class_<audio::fstp::PrefetchData>(m, "PrefetchData")
    .def_readwrite("start_frame", &audio::fstp::PrefetchData::start_frame)
    .def_readwrite("prefetch_size", &audio::fstp::PrefetchData::prefetch_size)
    .def_readwrite("reserved", &audio::fstp::PrefetchData::reserved);

  py::class_<audio::fstp::PrefetchDataBlock>(m, "PrefetchDataBlock")
    .def_readwrite("prefetch_metadata", &audio::fstp::PrefetchDataBlock::prefetch_metadata)
    .def_readwrite("sample_data", &audio::fstp::PrefetchDataBlock::sample_data);
}
} // namespace oead::bind