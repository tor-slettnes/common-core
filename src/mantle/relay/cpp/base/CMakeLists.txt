## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Relay - common data types
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_relay_common)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  relay-types.c++
  relay-publisher.c++
  relay-subscriber.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
