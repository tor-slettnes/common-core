## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(common)
add_subdirectory(platforms)

if(BUILD_TESTING)
  add_subdirectory(testing)
endif()

set(TARGET cc_core_inner)
set(LIB_DEPS
  cc_core_common
  cc_core_platform
)

cc_add_library("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)

