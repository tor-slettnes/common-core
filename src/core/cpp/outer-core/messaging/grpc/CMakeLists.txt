## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_grpc)

### Library type.  We use static linking for convenience.
set(LIB_TYPE INTERFACE)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_grpc_client
  cc_core_messaging_grpc_server
)

### Recepies to build the library
include(BuildLibrary)
