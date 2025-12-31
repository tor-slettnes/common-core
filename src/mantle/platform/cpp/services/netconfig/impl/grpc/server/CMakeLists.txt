## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
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

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
