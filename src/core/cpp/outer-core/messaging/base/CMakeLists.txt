## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build service API library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_base)

### Library dependencies.
set(LIB_DEPS
  cc_core_common
)

### Source files, roughly in dependency order
set(SOURCES
  messaging-endpoint.c++
)

## Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
