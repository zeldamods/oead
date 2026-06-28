cmake_minimum_required(VERSION 3.10)
project(oead CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-fcolor-diagnostics)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CMakeRC.cmake)
cmrc_add_resource_library(oead_res ALIAS oead::res NAMESPACE oead::res
  data/aglenv_file_info.json
  data/botw_hashed_names.txt
  data/botw_numbered_names.txt
  data/botw_resource_factory_info.tsv
)

include(FetchContent)

FetchContent_Declare(
  EasyIterator
  GIT_REPOSITORY https://github.com/TheLartians/EasyIterator
  GIT_TAG f3f89399ba8bca617040c1503f4f9fe3582a6abd
)
FetchContent_MakeAvailable(EasyIterator)

FetchContent_Declare(
  absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp
  GIT_TAG 35b9f25ba74a3cd71b6342168e74bd0dc4abf58b
)
FetchContent_MakeAvailable(absl)

FetchContent_Declare(
  yaml
  GIT_REPOSITORY https://github.com/yaml/libyaml
  GIT_TAG 893682bb98d5ed663a3e314c46dceaf9b1c8802f
)
FetchContent_MakeAvailable(yaml)

FetchContent_Declare(
  ordered_map
  GIT_REPOSITORY https://github.com/Tessil/ordered-map
  GIT_TAG 88b7a58f5e9be7f87998bbcbdd3865023738ef92
)
FetchContent_MakeAvailable(ordered_map)

FetchContent_Declare(
  ryml
  GIT_REPOSITORY https://github.com/biojppm/rapidyaml
  GIT_TAG 141041e4d1700376871fb83e773bbe3ec8b8dccd
)
FetchContent_MakeAvailable(ryml)

# Enabling AVX2 would require users to have a recent CPU
option(WITH_AVX2 "" OFF)
option(ZLIB_ENABLE_TESTS "" OFF)
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/zeldamods/zlib-ng
  GIT_TAG develop
)
FetchContent_MakeAvailable(zlib)

add_library(oead
  src/include/oead/util/align.h
  src/include/oead/util/binary_reader.h
  src/include/oead/util/bit_utils.h
  src/include/oead/util/hash.h
  src/include/oead/util/iterator_utils.h
  src/include/oead/util/magic_utils.h
  src/include/oead/util/scope_guard.h
  src/include/oead/util/string_utils.h
  src/include/oead/util/swap.h
  src/include/oead/util/type_utils.h
  src/include/oead/util/variant_utils.h
  src/include/oead/aamp.h
  src/include/oead/byml.h
  src/include/oead/errors.h
  src/include/oead/gsheet.h
  src/include/oead/sarc.h
  src/include/oead/types.h
  src/include/oead/yaz0.h
  src/aamp.cpp
  src/aamp_text.cpp
  src/byml.cpp
  src/byml_text.cpp
  src/gsheet.cpp
  src/sarc.cpp
  src/yml.cpp
  src/yml.h
  src/yaz0.cpp
)

target_include_directories(oead PUBLIC src/include)
target_include_directories(oead PRIVATE src/)
if (MSVC)
  target_compile_options(oead PRIVATE /W4 /wd4244 /wd4127 /Zc:__cplusplus)
else()
  target_compile_options(oead PRIVATE -Wall -Wextra -fno-plt)
endif()

target_include_directories(oead SYSTEM PUBLIC lib/nonstd)

set(BUILD_TESTING OFF)
target_link_libraries(oead
  PUBLIC
    absl::btree
    absl::flat_hash_map
    absl::hash
    EasyIterator
    tsl::ordered_map
  PRIVATE
    oead::res
    absl::inlined_vector
    absl::strings
    absl::str_format
    ryml
    yaml
    zlib
)
