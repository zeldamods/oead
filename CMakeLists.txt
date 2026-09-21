cmake_minimum_required(VERSION 3.15)
project(oead CXX)

include(CMakeRC.cmake)
cmrc_add_resource_library(oead_res ALIAS oead::res NAMESPACE oead::res
  data/aglenv_file_info.json
  data/botw_hashed_names.txt
  data/botw_numbered_names.txt
  data/botw_resource_factory_info.tsv
)

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
  src/yaml.cpp
  src/yaml.h
  src/yaz0.cpp
)

target_compile_features(oead PUBLIC cxx_std_20)
set_target_properties(oead PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(oead PUBLIC src/include)
target_include_directories(oead PRIVATE src/)
if (MSVC)
  target_compile_options(oead PRIVATE /W4 /wd4244 /wd4127 /Zc:__cplusplus)
else()
  target_compile_options(oead PRIVATE -Wall -Wextra -fno-plt)
endif()

target_include_directories(oead SYSTEM PUBLIC lib/nonstd)

function(oead_add_vendored_libraries)
  # libyaml requires CMake 3.10, so option() would otherwise ignore the variables below.
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(BUILD_TESTING OFF)
  set(WITH_GZFILEOP OFF)
  add_subdirectory(lib/abseil)
  add_subdirectory(lib/libyaml)
  add_subdirectory(lib/ordered-map)
  add_subdirectory(lib/rapidyaml)
  add_subdirectory(lib/zlib-ng)
endfunction()
oead_add_vendored_libraries()

target_link_libraries(oead
  PUBLIC
    absl::btree
    absl::flat_hash_map
    absl::hash
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
