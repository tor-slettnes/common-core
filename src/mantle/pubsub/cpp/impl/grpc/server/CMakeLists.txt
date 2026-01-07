## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Relay gRPC service
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_relay_grpc_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_relay_common
  cc_core_messaging_grpc_server
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_pubsub_proto
)


### Source files
set(SOURCES
  relay-grpc-requesthandler.c++
  relay-grpc-messagequeue.c++
  relay-grpc-run.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
