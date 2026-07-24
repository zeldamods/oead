#pragma once

namespace oead::audio {
struct ElementTypeCategory {
  enum {
    Tables = 0x100,
    Parameters = 0x200,
    Codecs = 0x300,
    General = 0x1F00,
  };

  enum class SoundArchiveFile {
    Blocks = 0x2000,
    InfoSections = 0x2100,
    ItemInfos = 0x2200,
    Parameters = 0x2300,
    General = 0x2400,
  };

  enum class StreamSoundFile {
    Blocks = 0x4000,
    ItemInfos = 0x4100,
  };

  enum class WaveSoundFile {
    Blocks = 0x4800,
    ItemInfos = 0x4900,
  };

  enum class SequenceSoundFile {
    Blocks = 0x5000,
    ItemInfos = 0x5100,
  };

  enum class BankFile {
    Blocks = 0x5800,
    Items = 0x5900,
    ItemTables = 0x6000,
  };

  enum class WaveArchiveFile {
    Blocks = 0x6800,
  };

  enum class WaveFile {
    Blocks = 0x7000,
    ItemInfos = 0x7100,
  };

  enum class GroupFile {
    Blocks = 0x7800,
    ItemInfos = 0x7900,
  };
  
  enum class AnimSoundFile {
    Blocks = 0x8000,
    Items = 0x8100,
  };
};

enum class ElementType {
  Blank = 0,
  Invalid = -1,

  Table_EmbeddingTable = ElementTypeCategory::Tables,
  Table_ReferenceTable,
  Table_ReferenceWithSizeTable,

  Parameter_Sound3d = ElementTypeCategory::Parameters,
  Parameter_Sends,
  Parameter_Envelope,
  Parameter_AdshrEnvelope,

  Codec_DspAdpcmInfo = ElementTypeCategory::Codecs,
  Codec_ImaAdpcmInfo,

  General_ByteStream = ElementTypeCategory::General,
  General_String,

  SoundArchiveFile_StringBlock = static_cast<int>(ElementTypeCategory::SoundArchiveFile::Blocks),
  SoundArchiveFile_InfoBlock,
  SoundArchiveFile_FileBlock,

  SoundArchiveFile_SoundInfoSection = static_cast<int>(ElementTypeCategory::SoundArchiveFile::InfoSections),
  SoundArchiveFile_BankInfoSection,
  SoundArchiveFile_PlayerInfoSection,
  SoundArchiveFile_WaveArchiveInfoSection,
  SoundArchiveFile_SoundGroupInfoSection,
  SoundArchiveFile_GroupInfoSection,
  SoundArchiveFile_FileInfoSection,

  SoundArchiveFile_SoundInfo = static_cast<int>(ElementTypeCategory::SoundArchiveFile::ItemInfos),
  SoundArchiveFile_StreamSoundInfo,
  SoundArchiveFile_WaveSoundInfo,
  SoundArchiveFile_SequenceSoundInfo,
  SoundArchiveFile_SoundGroupInfo,
  SoundArchiveFile_WaveSoundGroupInfo,
  SoundArchiveFile_BankInfo,
  SoundArchiveFile_WaveArchiveInfo,
  SoundArchiveFile_GroupInfo,
  SoundArchiveFile_PlayerInfo,
  SoundArchiveFile_FileInfo,
  SoundArchiveFile_SoundArchivePlayerInfo,
  SoundArchiveFile_InternalFileInfo,
  SoundArchiveFile_ExternalFileInfo,
  SoundArchiveFile_StreamSoundTrackInfo,
  SoundArchiveFile_SendInfo,
  SoundArchiveFile_StreamSoundExtensionInfo,

  SoundArchiveFile_StringTable = static_cast<int>(ElementTypeCategory::SoundArchiveFile::General),
  SoundArchiveFile_PatriciaTree,

  StreamSoundFile_InfoBlock = static_cast<int>(ElementTypeCategory::StreamSoundFile::Blocks),
  StreamSoundFile_SeekBlock,
  StreamSoundFile_DataBlock,
  StreamSoundFile_RegionBlock,
  StreamSoundFile_PrefetchDataBlock,

  StreamSoundFile_StreamSoundInfo = static_cast<int>(ElementTypeCategory::StreamSoundFile::ItemInfos),
  StreamSoundFile_TrackInfo,
  StreamSoundFile_ChannelInfo,

  WaveSoundFile_WaveSoundMetaData = static_cast<int>(ElementTypeCategory::WaveSoundFile::ItemInfos),
  WaveSoundFile_WaveSoundInfo,
  WaveSoundFile_NoteInfo,
  WaveSoundFile_TrackInfo,
  WaveSoundFile_NoteEvent,

  SequenceSoundFile_DataBlock = static_cast<int>(ElementTypeCategory::SequenceSoundFile::Blocks),
  SequenceSoundFile_LabelBlock,

  SequenceSoundFile_LabelInfo = static_cast<int>(ElementTypeCategory::SequenceSoundFile::ItemInfos),

  BankFile_InfoBlock = static_cast<int>(ElementTypeCategory::BankFile::Blocks),

  BankFile_InstrumentInfo = static_cast<int>(ElementTypeCategory::BankFile::Items),
  BankFile_KeyRegionInfo,
  BankFile_VelocityRegionInfo,
  BankFile_NullInfo,

  BankFile_DirectReferenceTable = static_cast<int>(ElementTypeCategory::BankFile::ItemTables),
  BankFile_RangeReferenceTable,
  BankFile_IndexReferenceTable,

  WaveArchiveFile_InfoBlock = static_cast<int>(ElementTypeCategory::WaveArchiveFile::Blocks),
  
  WaveFile_InfoBlock = static_cast<int>(ElementTypeCategory::WaveFile::Blocks),
  WaveFile_DataBlock,

  WaveFile_ChannelInfo = static_cast<int>(ElementTypeCategory::WaveFile::ItemInfos),

  GroupFile_InfoBlock = static_cast<int>(ElementTypeCategory::GroupFile::Blocks),
  GroupFile_FileBlock,
  GroupFile_InfoExBlock,

  GroupFile_GroupItemInfo = static_cast<int>(ElementTypeCategory::GroupFile::ItemInfos),
  GroupFile_GroupItemInfoEx,

  AnimSoundFile_DataBlock = static_cast<int>(ElementTypeCategory::AnimSoundFile::Blocks),

  AnimSoundFile_EventInfo = static_cast<int>(ElementTypeCategory::AnimSoundFile::Items),
};
} // oead::audio