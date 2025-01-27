## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(base)
add_subdirectory(server)
add_subdirectory(client)

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_grpc)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_grpc_client
  cc_core_messaging_grpc_server
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)
