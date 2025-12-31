## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  reader.c++
  writer.c++
)

add_subdirectory(custom)

if(BUILD_RAPIDJSON)
  add_subdirectory(rapidjson)
endif()

