## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_netconfig_grpc_service")

set(LIB_DEPS
  cc_platform_netconfig_base
  cc_platform_netconfig_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  netconfig-grpc-signalqueue.c++
  netconfig-grpc-requesthandler.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
