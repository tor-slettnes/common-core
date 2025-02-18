## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Logging gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_multilogger_grpc_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_multilogger_base
  cc_multilogger_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  multilogger-grpc-requesthandler.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
