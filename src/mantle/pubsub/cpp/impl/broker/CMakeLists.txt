## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Relay - broker-side implementation
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_relay_broker)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_relay_common
)

set(SOURCES
  broker-relay-control.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
