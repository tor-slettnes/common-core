## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_grpc_server)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_grpc_base
)

set(SOURCES
  grpc-requesthandler.c++
  grpc-serverbuilder.c++
  grpc-serverinterceptors.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
