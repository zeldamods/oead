#include <oead/audio/fstm.h>

#include "main.h"

namespace oead::bind {
void BindFstm(py::module& parent) {
  py::module m = parent.def_submodule("audio");

  py::class_<audio::fstm::Fstm>(m, "Fstm")
    .def(py::init<tcb::span<const u8>>())
    .def("info", &audio::fstm::Fstm::Info)
    .def("has_region", &audio::fstm::Fstm::HasRegion)
    .def("endian", py::overload_cast<util::Endianness>(&audio::fstm::Fstm::Endianness))
    .def("endian", py::overload_cast<>(&audio::fstm::Fstm::Endianness, py::const_))
    .def("to_binary", &audio::fstm::Fstm::ToBinary);

  py::class_<audio::fstm::InfoBlock>(m, "StreamInfoBlock")
    .def("stream_info", &audio::fstm::InfoBlock::StreamInfo)
    .def("track_infos", &audio::fstm::InfoBlock::TrackInfos)
    .def("channel_infos", &audio::fstm::InfoBlock::DetailChannelInfos);

  py::class_<audio::fstm::RegionInfo>(m, "StreamRegionInfo")
    .def_readwrite("start", &audio::fstm::RegionInfo::start)
    .def_readwrite("end", &audio::fstm::RegionInfo::end)
    .def_readwrite("adpcm_context", &audio::fstm::RegionInfo::adpcm_context)
    .def_readwrite("is_enabled", &audio::fstm::RegionInfo::is_enabled)
    .def_readwrite("region_name", &audio::fstm::RegionInfo::region_name);

  py::class_<audio::fstm::SeekInfo>(m, "StreamSeekInfo")
    .def_readwrite("yn1", &audio::fstm::SeekInfo::yn1)
    .def_readwrite("yn2", &audio::fstm::SeekInfo::yn2);

  py::class_<audio::fstm::StreamSoundInfo>(m, "StreamSoundInfo")
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

  py::class_<audio::fstm::TrackInfo>(m, "TrackInfo")
    .def_readwrite("volume", &audio::fstm::TrackInfo::volume)
    .def_readwrite("pan", &audio::fstm::TrackInfo::pan)
    .def_readwrite("span", &audio::fstm::TrackInfo::span)
    .def_readwrite("flags", &audio::fstm::TrackInfo::flags);
}
} // namespace oead::bind