## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Logging gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_logger_grpc_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_logger_base
  cc_logger_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  # logger-grpc-messagequeue.c++
  logger-grpc-requesthandler.c++
)

cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
