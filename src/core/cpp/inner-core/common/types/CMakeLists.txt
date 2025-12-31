## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  streamable.c++
  listable.c++
  loggable.c++
  bytevector.c++
  variant-types.c++
  variant-value.c++
  variant-list.c++
  variant-tvlist.c++
  variant-kvmap.c++
)
