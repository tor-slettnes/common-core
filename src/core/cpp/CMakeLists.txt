## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(inner-core)
add_subdirectory(outer-core)

cc_add_doxygen(c++-doc
  FILE_PATTERNS *.c++ *.h++ *.i++ *.md)
