## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  reader.c++
  writer.c++
)

add_subdirectory(custom)

if(BUILD_RAPIDJSON)
  add_subdirectory(rapidjson)
endif()

