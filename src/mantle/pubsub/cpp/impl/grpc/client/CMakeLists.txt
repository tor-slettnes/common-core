## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Relay gRPC clients
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_relay_grpc_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_relay_common
  cc_core_messaging_grpc_client)

set(PROTO_DEPS
  cc_pubsub_proto
)

### Source files
set(SOURCES
  relay-grpc-client.c++
  relay-grpc-reader.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
