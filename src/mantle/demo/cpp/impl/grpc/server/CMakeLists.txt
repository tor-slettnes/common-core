## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Demo gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_grpc_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protobuf_types
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  demo-grpc-signalqueue.c++
  demo-grpc-requesthandler.c++
  demo-grpc-run.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
