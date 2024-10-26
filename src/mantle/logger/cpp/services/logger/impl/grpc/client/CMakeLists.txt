## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build logger gRPC client library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_logger_grpc_client")

set(LIB_DEPS
  cc_logger_base
  cc_logger_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  logger-grpc-client.c++
)

# cc_add_library("${TARGET}"
#   LIB_DEPS ${LIB_DEPS}
#   SOURCES ${SOURCES}
# )
