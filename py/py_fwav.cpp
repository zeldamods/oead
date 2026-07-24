#include <oead/audio/fwav.h>

#include "main.h"

namespace oead::bind {
void BindFwav(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fwav::Fwav, std::shared_ptr<audio::fwav::Fwav>, audio::IAssetFile>(m, "Fwav")
    .def(py::init<tcb::span<const u8>>())
    .def_property("encoding", &audio::fwav::Fwav::GetEncoding, &audio::fwav::Fwav::SetEncoding)
    .def("channel_infos", &audio::fwav::Fwav::ChannelInfos)
    .def("samples", &audio::fwav::Fwav::Samples)
    .def("endian", py::overload_cast<util::Endianness>(&audio::fwav::Fwav::Endianness))
    .def("endian", py::overload_cast<>(&audio::fwav::Fwav::Endianness, py::const_))
    .def("to_binary", &audio::fwav::Fwav::ToBinary);

  py::class_<audio::fwav::WaveInfo>(m, "WaveInfo")
    .def_readwrite("encoding", &audio::fwav::WaveInfo::encoding)
    .def_readwrite("is_loop", &audio::fwav::WaveInfo::is_loop)
    .def_readwrite("sample_rate", &audio::fwav::WaveInfo::sample_rate)
    .def_readwrite("loop_start_frame", &audio::fwav::WaveInfo::loop_start_frame)
    .def_readwrite("loop_end_frame", &audio::fwav::WaveInfo::loop_end_frame)
    .def_readwrite("original_loop_start_frame", &audio::fwav::WaveInfo::original_loop_start_frame);

  py::class_<audio::fwav::ChannelInfo>(m, "ChannelInfo")
    .def_readwrite("adpcm_info", &audio::fwav::ChannelInfo::adpcm_info);
}
} // namespace oead::bind