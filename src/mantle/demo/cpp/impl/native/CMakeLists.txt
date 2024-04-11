## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build native implementation of Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_demo_native)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_demo_api)

set(SOURCES
  demo-native.c++)

## Invoke common CMake rules
build_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
