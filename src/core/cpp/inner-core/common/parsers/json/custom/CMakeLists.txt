## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  parserinput-stream.c++
  parserinput-string.c++
  tokenparser.c++
  reader.c++
  writer.c++
)

