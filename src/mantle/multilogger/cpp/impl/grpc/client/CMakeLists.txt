## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build multilogger gRPC client library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_multilogger_grpc_client")

set(LIB_DEPS
  cc_multilogger_base
  cc_multilogger_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  multilogger-grpc-client.c++
  multilogger-grpc-clientlistener.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
