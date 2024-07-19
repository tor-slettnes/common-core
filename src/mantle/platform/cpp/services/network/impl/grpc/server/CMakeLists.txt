## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_network_grpc_service")

set(LIB_DEPS
  cc_platform_network_base
  cc_platform_network_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  network-grpc-signalqueue.c++
  network-grpc-requesthandler.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
