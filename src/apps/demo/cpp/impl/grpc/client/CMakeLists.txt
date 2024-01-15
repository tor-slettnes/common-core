## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Demo gRPC clients
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_grpc_client)

### What kind of library we are building (STATIC|SHARED|OBJECT|...); the default
### is STATIC.  See:
### https://cmake.org/cmake/help/latest/command/add_library.html.
set(LIB_TYPE STATIC)

### External package dependencies, configured via `pkg-config`.
set(PKG_DEPS
  grpc++
)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protobuf
  cc_shared_ipc_grpc)

### Source files
set(SOURCES
  demo-grpc-client.c++
)


### Invoke common CMake rules
include(BuildLibrary)
