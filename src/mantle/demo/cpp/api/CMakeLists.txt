## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_demo_api)

### Make this an OBJECT library whose files will be included direcly in dowstream dependants
set(LIB_TYPE STATIC)

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
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
