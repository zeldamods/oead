#include <oead/audio/amta.h>

#include "main.h"

namespace oead::bind {
void BindAmta(py::module& parent) {
  py::module m = parent.def_submodule("audio");
  
  py::class_<audio::amta::Amta>(m, "Amta")
    .def(py::init<tcb::span<const u8>>())
    .def("asset_name", &audio::amta::Amta::AssetName)
    .def("markers", &audio::amta::Amta::Markers)
    .def("ext", &audio::amta::Amta::ExtValues)
    .def("version", &audio::amta::Amta::Version)
    .def("to_binary", &audio::amta::Amta::ToBinary);

  py::class_<audio::amta::AudioMetaDataBin::StreamTrack>(m, "StreamTrack")
    .def_readwrite("channel_count", &audio::amta::AudioMetaDataBin::StreamTrack::channel_count)
    .def_readwrite("volume", &audio::amta::AudioMetaDataBin::StreamTrack::volume);

  py::class_<audio::amta::AudioMetaDataBin>(m, "AudioMetaData")
    .def_readwrite("header", &audio::amta::AudioMetaDataBin::header)
    .def_readwrite("asset_name_offset", &audio::amta::AudioMetaDataBin::asset_name_offset)
    .def_readwrite("sample_count", &audio::amta::AudioMetaDataBin::sample_count)
    .def_readwrite("type", &audio::amta::AudioMetaDataBin::type)
    .def_readwrite("channel_count", &audio::amta::AudioMetaDataBin::channel_count)
    .def_readwrite("used_stream_tracks", &audio::amta::AudioMetaDataBin::used_stream_tracks)
    .def_readwrite("flags", &audio::amta::AudioMetaDataBin::flags)
    .def_readwrite("duration", &audio::amta::AudioMetaDataBin::duration)
    .def_readwrite("sample_rate", &audio::amta::AudioMetaDataBin::sample_rate)
    .def_readwrite("loop_start_frame", &audio::amta::AudioMetaDataBin::loop_start_frame)
    .def_readwrite("loop_end_frame", &audio::amta::AudioMetaDataBin::loop_end_frame)
    .def_readwrite("loudness", &audio::amta::AudioMetaDataBin::loudness)
    .def_readwrite("stream_tracks", &audio::amta::AudioMetaDataBin::stream_tracks)
    .def_readwrite("amplitude_peak", &audio::amta::AudioMetaDataBin::amplitude_peak);
}
} // namespace oead::bind