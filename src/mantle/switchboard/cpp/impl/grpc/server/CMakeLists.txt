## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Switchboard gRPC service
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_switchboard_grpc_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_switchboard_base
  cc_switchboard_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  switchboard-grpc-signalqueue.c++
  switchboard-grpc-requesthandler.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
