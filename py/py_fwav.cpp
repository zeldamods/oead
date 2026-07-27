#include <oead/audio/fwav.h>
#include <pybind11/detail/common.h>

#include "main.h"

namespace oead::bind {
void BindFwav(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fwav::Fwav, std::shared_ptr<audio::fwav::Fwav>, audio::IAssetFile>(m, "Fwav")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def("to_binary", py::overload_cast<>(&audio::fwav::Fwav::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::fwav::Fwav::ToBinary, py::const_))
    .def_property(
      "encoding", 
      py::overload_cast<>(&audio::fwav::Fwav::Encoding, py::const_), 
      py::overload_cast<audio::SampleFormat>(&audio::fwav::Fwav::Encoding)
    )
    .def_property(
      "is_loop", 
      py::overload_cast<>(&audio::fwav::Fwav::isLoop, py::const_), 
      py::overload_cast<bool>(&audio::fwav::Fwav::isLoop)
    )
    .def_property(
      "sample_rate", 
      py::overload_cast<>(&audio::fwav::Fwav::SampleRate, py::const_), 
      py::overload_cast<u32>(&audio::fwav::Fwav::SampleRate)
    )
    .def_property(
      "loop_start_frame", 
      py::overload_cast<>(&audio::fwav::Fwav::LoopStartFrame, py::const_), 
      py::overload_cast<u32>(&audio::fwav::Fwav::LoopStartFrame)
    )
    .def_property(
      "loop_end_frame", 
      py::overload_cast<>(&audio::fwav::Fwav::LoopEndFrame, py::const_), 
      py::overload_cast<u32>(&audio::fwav::Fwav::LoopEndFrame)
    )
    .def_property(
      "original_loop_start_frame", 
      py::overload_cast<>(&audio::fwav::Fwav::OriginalLoopStartFrame, py::const_), 
      py::overload_cast<u32>(&audio::fwav::Fwav::OriginalLoopStartFrame)
    )
    .def_property(
      "channel_infos", 
      py::overload_cast<>(&audio::fwav::Fwav::ChannelInfos, py::const_), 
      py::overload_cast<const std::vector<audio::fwav::ChannelInfo>&>(&audio::fwav::Fwav::ChannelInfos)
    )
    .def("get_channel_info", &audio::fwav::Fwav::GetChannelInfo)
    .def("set_channel_info", &audio::fwav::Fwav::SetChannelInfo)
    .def_property(
      "samples", 
      py::overload_cast<>(&audio::fwav::Fwav::Samples, py::const_), 
      py::overload_cast<const std::vector<audio::Channel>&>(&audio::fwav::Fwav::Samples)
    )
    .def_property(
      "endian", 
      py::overload_cast<>(&audio::fwav::Fwav::Endianness, py::const_), 
      py::overload_cast<util::Endianness>(&audio::fwav::Fwav::Endianness)
    );

  py::class_<audio::fwav::InfoBlock>(m, "WaveInfo")
    .def(py::init<>())
    .def_readwrite("encoding", &audio::fwav::InfoBlock::encoding)
    .def_readwrite("is_loop", &audio::fwav::InfoBlock::is_loop)
    .def_readwrite("sample_rate", &audio::fwav::InfoBlock::sample_rate)
    .def_readwrite("loop_start_frame", &audio::fwav::InfoBlock::loop_start_frame)
    .def_readwrite("loop_end_frame", &audio::fwav::InfoBlock::loop_end_frame)
    .def_readwrite("original_loop_start_frame", &audio::fwav::InfoBlock::original_loop_start_frame);

  py::class_<audio::fwav::ChannelInfo>(m, "ChannelInfo")
    .def(py::init<>())
    .def_readwrite("adpcm_info", &audio::fwav::ChannelInfo::adpcm_info);
}
} // namespace oead::bind