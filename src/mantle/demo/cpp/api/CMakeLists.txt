## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_demo_api)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_core
)

set(SOURCES
  demo-types.c++
  demo-signals.c++
  demo-api.c++
  demo-provider.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
