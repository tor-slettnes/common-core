## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build platform-specific code - QNX version
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_platform_qnx)

### Library dependencies
set(LIB_DEPS
  cc_core_platform_posix
)

### Sources
set(SOURCES
  qnx-providers.c++
  qnx-path.c++
)

### Build library
include(cc_add_library)
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
