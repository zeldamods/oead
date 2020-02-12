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
  data/botw_hashed_names.txt
  data/botw_numbered_names.txt
)

add_library(oead
  src/util/align.h
  src/util/binary_reader.h
  src/util/bit_utils.h
  src/util/hash.h
  src/util/iterator_utils.h
  src/util/scope_guard.h
  src/util/swap.h
  src/util/type_utils.h
  src/util/types.h
  src/util/variant_utils.h
  src/include/oead/aamp.h
  src/include/oead/byml.h
  src/include/oead/errors.h
  src/include/oead/types.h
  src/aamp.cpp
  src/aamp_text.cpp
  src/byml.cpp
  src/byml_text.cpp
  src/yaml.cpp
  src/yaml.h
)

target_include_directories(oead PUBLIC src/include)
target_include_directories(oead PRIVATE src/)
if (MSVC)
  target_compile_options(oead PRIVATE /W4)
  # https://github.com/abseil/abseil-cpp/issues/357
  target_compile_options(oead PUBLIC /permissive)
else()
  target_compile_options(oead PRIVATE -Wall -Wextra -fno-plt)
endif()

target_include_directories(oead SYSTEM PUBLIC lib/nonstd)

set(BUILD_TESTING OFF)
add_subdirectory(lib/abseil)
add_subdirectory(lib/libyaml)
add_subdirectory(lib/ordered-map)
add_subdirectory(lib/rapidyaml)
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
)
