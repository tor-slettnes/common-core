## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard gRPC client library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_switchboard_grpc_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_switchboard_base
  cc_switchboard_protobuf
  cc_core_messaging_grpc_client
)

### Source files
set(SOURCES
  remote-grpc-switch.c++
  switchboard-grpc-proxy.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
