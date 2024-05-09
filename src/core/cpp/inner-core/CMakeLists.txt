## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(common)
add_subdirectory(platforms)
add_subdirectory(testing)

set(TARGET cc_core_inner)
set(LIB_DEPS
  cc_core_common
  cc_core_platform
)

BuildLibrary("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)

