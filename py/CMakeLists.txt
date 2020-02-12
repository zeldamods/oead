cmake_minimum_required(VERSION 3.10)
project(oead CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR MINGW)
  # GNU ar: Create thin archive files.
  # Requires binutils-2.19 or later.
  set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> qcTP <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_C_ARCHIVE_APPEND   "<CMAKE_AR> qTP  <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcTP <TARGET> <LINK_FLAGS> <OBJECTS>")
  set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> qTP  <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()

if (MSVC)
  set(PYBIND11_CPP_STANDARD /std:c++latest)
else()
  set(PYBIND11_CPP_STANDARD -std=c++17)
endif()

add_subdirectory(../lib/pybind11 pybind11)

pybind11_add_module(oead-py
  main.h
  main.cpp
  py_aamp.cpp
  py_byml.cpp
  py_common_types.cpp
  pybind11_common.h
  pybind11_variant_caster.h
)
set_target_properties(oead-py PROPERTIES OUTPUT_NAME oead)
add_subdirectory(../ oead)
target_link_libraries(oead-py PRIVATE oead)
