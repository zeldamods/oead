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

find_program(CCACHE_BIN ccache)
if(CCACHE_BIN)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_BIN})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_BIN})
  # ccache uses -I when compiling without preprocessor, which makes clang complain.
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Qunused-arguments -fcolor-diagnostics")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
  endif()
endif()

if (MSVC)
  set(PYBIND11_CPP_STANDARD /std:c++17)
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
  py_gsheet.cpp
  py_sarc.cpp
  py_yaz0.cpp
  pybind11_common.h
  pybind11_variant_caster.h
)
set_target_properties(oead-py PROPERTIES OUTPUT_NAME oead)
add_subdirectory(../ oead)
target_link_libraries(oead-py PRIVATE oead)
