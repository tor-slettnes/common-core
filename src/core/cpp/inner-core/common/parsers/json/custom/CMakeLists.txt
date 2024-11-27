## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  parserinput-stream.c++
  parserinput-string.c++
  tokenparser.c++
  reader.c++
  writer.c++
)

