## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Demo gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_grpc_service)

### What kind of library we are building (STATIC|SHARED|OBJECT|...); the default
### is STATIC.  See:
### https://cmake.org/cmake/help/latest/command/add_library.html.
set(LIB_TYPE STATIC)

# ### External package dependencies, configured via `pkg-config`.
# set(PKG_DEPS
#   grpc++
# )

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protobuf_types
  cc_core_messaging_grpc)

### Source files
set(SOURCES
  demo-grpc-signalqueue.c++
  demo-grpc-requesthandler.c++
  demo-grpc-run.c++
)


### Invoke common CMake rules
include(BuildLibrary)
