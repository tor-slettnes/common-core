## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Demo gRPC clients
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_grpc_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protobuf_types
  cc_core_messaging_grpc_client)

### Source files
set(SOURCES
  demo-grpc-client.c++
)


BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
