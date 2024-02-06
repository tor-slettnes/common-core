## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  streamable.c++
  bytevector.c++
  partslist.c++
  variant-types.c++
  variant-value.c++
  variant-list.c++
  variant-tvlist.c++
  variant-kvmap.c++
)
