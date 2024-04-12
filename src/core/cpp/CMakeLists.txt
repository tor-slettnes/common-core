## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildLibrary)
add_subdirectory(inner-core)
add_subdirectory(outer-core)
