## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_grpc_client)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_grpc_common
  cc_core_messaging_http
)

set(SOURCES
  grpc-clientbase.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
