#include <oead/audio/amta.h>

#include "main.h"

namespace oead::bind {
void BindAmta(py::module& parent) {
  py::module m = parent.def_submodule("audio");
  
  py::class_<audio::amta::Amta>(m, "Amta")
    .def(py::init<>())
    .def(py::init<tcb::span<const u8>>())
    .def("to_binary", py::overload_cast<>(&audio::amta::Amta::ToBinary, py::const_))
    .def("to_binary", py::overload_cast<util::Endianness>(&audio::amta::Amta::ToBinary, py::const_))
    .def_property(
      "asset_name", 
      py::overload_cast<>(&audio::amta::Amta::AssetName, py::const_),
      py::overload_cast<std::string>(&audio::amta::Amta::AssetName) 
    )
    .def_property(
      "version",
      py::overload_cast<>(&audio::amta::Amta::Version, py::const_),
      py::overload_cast<u16>(&audio::amta::Amta::Version)
    )
    .def_property(
      "sample_count",
      py::overload_cast<>(&audio::amta::Amta::SampleCount, py::const_),
      py::overload_cast<u32>(&audio::amta::Amta::SampleCount)
    )
    .def_property(
      "sample_count",
      py::overload_cast<>(&audio::amta::Amta::SampleCount, py::const_),
      py::overload_cast<u32>(&audio::amta::Amta::SampleCount) 
    )
    .def_property(
      "type",
      py::overload_cast<>(&audio::amta::Amta::Type, py::const_),
      py::overload_cast<audio::AssetType>(&audio::amta::Amta::Type)
    )
    .def_property(
      "channel_count",
      py::overload_cast<>(&audio::amta::Amta::ChannelCount, py::const_),
      py::overload_cast<u8>(&audio::amta::Amta::ChannelCount)
    )
    .def_property(
      "used_stream_tracks",
      py::overload_cast<>(&audio::amta::Amta::UsedStreamTracks, py::const_),
      py::overload_cast<u8>(&audio::amta::Amta::UsedStreamTracks)
    )
    .def_property(
      "flags",
      py::overload_cast<>(&audio::amta::Amta::Flags, py::const_),
      py::overload_cast<u8>(&audio::amta::Amta::Flags)
    )
    .def_property(
      "unknown",
      py::overload_cast<>(&audio::amta::Amta::Unknown, py::const_),
      py::overload_cast<float>(&audio::amta::Amta::Unknown)
    )
    .def_property(
      "sample_rate",
      py::overload_cast<>(&audio::amta::Amta::SampleRate, py::const_),
      py::overload_cast<u32>(&audio::amta::Amta::SampleRate)
    )
    .def_property(
      "loop_start_frame",
      py::overload_cast<>(&audio::amta::Amta::LoopStartFrame, py::const_),
      py::overload_cast<u32>(&audio::amta::Amta::LoopStartFrame)
    )
    .def_property(
      "loop_end_frame",
      py::overload_cast<>(&audio::amta::Amta::LoopEndFrame, py::const_),
      py::overload_cast<u32>(&audio::amta::Amta::LoopEndFrame)
    )
    .def_property(
      "volume",
      py::overload_cast<>(&audio::amta::Amta::Volume, py::const_),
      py::overload_cast<float>(&audio::amta::Amta::Volume)
    )
    .def_property(
      "stream_tracks",
      py::overload_cast<>(&audio::amta::Amta::StreamTracks, py::const_),
      py::overload_cast<std::array<audio::amta::Amta::StreamTrack, 8>>(&audio::amta::Amta::StreamTracks)
    )
    .def("get_stream_track", &audio::amta::Amta::GetStreamTrack, "idx"_a)
    .def("set_stream_track", &audio::amta::Amta::SetStreamTrack, "track"_a, "idx"_a)
    .def_property(
      "amplitude_peak",
      py::overload_cast<>(&audio::amta::Amta::AmplitudePeak, py::const_),
      py::overload_cast<float>(&audio::amta::Amta::AmplitudePeak)
    )
    .def_property(
      "markers",
      py::overload_cast<>(&audio::amta::Amta::Markers, py::const_),
      py::overload_cast<const std::vector<audio::amta::MarkerInfo>&>(&audio::amta::Amta::Markers)
    )
    .def("get_marker", &audio::amta::Amta::GetMarker, "idx"_a)
    .def("set_marker", &audio::amta::Amta::SetMarker, "marker"_a, "idx"_a)
    .def_property(
      "ext_entries",
      py::overload_cast<>(&audio::amta::Amta::ExtEntries, py::const_),
      py::overload_cast<const std::vector<audio::amta::ExtEntry>&>(&audio::amta::Amta::ExtEntries)
    )
    .def("get_ext_entry", &audio::amta::Amta::GetExtEntry, "idx"_a)
    .def("set_ext_entry", &audio::amta::Amta::SetExtEntry, "ext"_a, "idx"_a)
    .def_property(
      "endian",
      py::overload_cast<>(&audio::amta::Amta::Endianness, py::const_),
      py::overload_cast<util::Endianness>(&audio::amta::Amta::Endianness)
    );

  py::class_<audio::amta::Amta::StreamTrack>(m, "StreamTrack")
    .def_readwrite("channel_count", &audio::amta::Amta::StreamTrack::channel_count)
    .def_readwrite("volume", &audio::amta::Amta::StreamTrack::volume);
}
} // namespace oead::bind