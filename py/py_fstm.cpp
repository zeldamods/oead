#include <oead/audio/fstm.h>

#include "main.h"

namespace oead::bind {
void BindFstm(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fstm::Fstm>(m, "Fstm")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def("to_binary", py::overload_cast<>(&audio::fstm::Fstm::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::fstm::Fstm::ToBinary, py::const_), "endian"_a)
    .def_property(
      "info", 
      py::overload_cast<>(&audio::fstm::Fstm::Info, py::const_),
      py::overload_cast<audio::fstm::InfoBlock>(&audio::fstm::Fstm::Info)
    )
    .def_property(
      "seek_infos",
      py::overload_cast<>(&audio::fstm::Fstm::SeekInfos, py::const_),
      py::overload_cast<const std::vector<std::vector<audio::fstm::SeekInfo>>&>(&audio::fstm::Fstm::SeekInfos)
    )
    .def("get_seek_info", &audio::fstm::Fstm::GetSeekInfo, "block"_a, "channel"_a)
    .def("set_seek_info", &audio::fstm::Fstm::SetSeekInfo, "info"_a, "block"_a, "channel"_a)
    .def_property(
      "region_infos",
      py::overload_cast<>(&audio::fstm::Fstm::RegionInfos, py::const_),
      py::overload_cast<const std::vector<audio::fstm::RegionInfo>&>(&audio::fstm::Fstm::RegionInfos)
    )
    .def("get_region_info", &audio::fstm::Fstm::GetRegionInfo, "id"_a)
    .def("set_region_info", &audio::fstm::Fstm::SetRegionInfo, "info"_a, "id"_a)
    .def_property(
      "samples",
      py::overload_cast<>(&audio::fstm::Fstm::Samples, py::const_),
      py::overload_cast<const std::vector<audio::Channel>&>(&audio::fstm::Fstm::Samples)
    )
    .def("has_region", &audio::fstm::Fstm::HasRegion)
    .def_property(
      "endian",
      py::overload_cast<>(&audio::fstm::Fstm::Endianness, py::const_),
      py::overload_cast<util::Endianness>(&audio::fstm::Fstm::Endianness)
    );

  py::class_<audio::fstm::InfoBlock>(m, "StreamInfoBlock")
    .def(py::init<>())
    .def_property(
      "stream_info", 
      py::overload_cast<>(&audio::fstm::InfoBlock::StreamInfo, py::const_),
      py::overload_cast<audio::fstm::StreamSoundInfo>(&audio::fstm::InfoBlock::StreamInfo)
    )
    .def_property(
      "track_infos", 
      py::overload_cast<>(&audio::fstm::InfoBlock::TrackInfos, py::const_),
      py::overload_cast<const std::vector<audio::fstm::TrackInfo>&>(&audio::fstm::InfoBlock::TrackInfos)
    )
    .def("get_track_info", &audio::fstm::InfoBlock::GetTrackInfo, "id"_a)
    .def("set_track_info", &audio::fstm::InfoBlock::SetTrackInfo, "info"_a, "id"_a)
    .def_property(
      "channel_infos", 
      py::overload_cast<>(&audio::fstm::InfoBlock::DetailChannelInfos, py::const_),
      py::overload_cast<const std::vector<audio::DspAdpcmInfo>&>(&audio::fstm::InfoBlock::DetailChannelInfos)
    )
    .def("get_channel_info", &audio::fstm::InfoBlock::GetDetailChannelInfo, "channel"_a)
    .def("set_channel_info", &audio::fstm::InfoBlock::SetDetailChannelInfo, "info"_a, "channel"_a);

  py::class_<audio::fstm::StreamSoundInfo>(m, "StreamSoundInfo")
    .def(py::init<>())
    .def_readwrite("encoding", &audio::fstm::StreamSoundInfo::encoding)
    .def_readwrite("is_loop", &audio::fstm::StreamSoundInfo::is_loop)
    .def_readwrite("channel_count", &audio::fstm::StreamSoundInfo::channel_count)
    .def_readwrite("region_count", &audio::fstm::StreamSoundInfo::region_count)
    .def_readwrite("sample_rate", &audio::fstm::StreamSoundInfo::sample_rate)
    .def_readwrite("loop_start", &audio::fstm::StreamSoundInfo::loop_start)
    .def_readwrite("frame_count", &audio::fstm::StreamSoundInfo::frame_count)
    .def_readwrite("block_count", &audio::fstm::StreamSoundInfo::block_count)
    .def_readwrite("block_size", &audio::fstm::StreamSoundInfo::block_size)
    .def_readwrite("block_sample_count", &audio::fstm::StreamSoundInfo::block_sample_count)
    .def_readwrite("last_block_size", &audio::fstm::StreamSoundInfo::last_block_size)
    .def_readwrite("last_block_sample_count", &audio::fstm::StreamSoundInfo::last_block_sample_count)
    .def_readwrite("last_block_padding_size", &audio::fstm::StreamSoundInfo::last_block_padding_size)
    .def_readwrite("size_of_seek_info_atom", &audio::fstm::StreamSoundInfo::size_of_seek_info_atom)
    .def_readwrite("seek_info_interval_samples", &audio::fstm::StreamSoundInfo::seek_info_interval_samples)
    .def_readwrite("region_info_size", &audio::fstm::StreamSoundInfo::region_info_size)
    .def_readwrite("original_loop_start", &audio::fstm::StreamSoundInfo::original_loop_start)
    .def_readwrite("original_loop_end", &audio::fstm::StreamSoundInfo::original_loop_end);

  py::class_<audio::fstm::SeekInfo>(m, "StreamSeekInfo")
    .def(py::init<>())
    .def_readwrite("yn1", &audio::fstm::SeekInfo::yn1)
    .def_readwrite("yn2", &audio::fstm::SeekInfo::yn2);

  py::class_<audio::fstm::RegionInfo>(m, "StreamRegionInfo")
    .def(py::init<>())
    .def_readwrite("start", &audio::fstm::RegionInfo::start)
    .def_readwrite("end", &audio::fstm::RegionInfo::end)
    .def_readwrite("adpcm_context", &audio::fstm::RegionInfo::adpcm_context)
    .def_readwrite("is_enabled", &audio::fstm::RegionInfo::is_enabled)
    .def_readwrite("region_name", &audio::fstm::RegionInfo::region_name);

  py::class_<audio::fstm::TrackInfo>(m, "StreamTrackInfo")
    .def(py::init<>())
    .def_readwrite("volume", &audio::fstm::TrackInfo::volume)
    .def_readwrite("pan", &audio::fstm::TrackInfo::pan)
    .def_readwrite("span", &audio::fstm::TrackInfo::span)
    .def_readwrite("flags", &audio::fstm::TrackInfo::flags);
}
} // namespace oead::bind