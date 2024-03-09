## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  tokenparser-base.c++
  tokenparser-stream.c++
  tokenparser-string.c++
  jsonparser.c++
  jsonbuilder.c++
  jsonwriter.c++
)

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
