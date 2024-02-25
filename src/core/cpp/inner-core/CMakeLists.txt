## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(common)
add_subdirectory(platforms)

set(TARGET cc_core_inner)
set(LIB_TYPE INTERFACE)
set(LIB_DEPS
  cc_core_common
  cc_core_platform
)

include(BuildLibrary)
