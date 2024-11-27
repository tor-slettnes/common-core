## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  basereader.c++
  basewriter.c++
)
