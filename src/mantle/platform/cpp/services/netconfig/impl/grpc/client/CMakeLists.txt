## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_netconfig_grpc_client")

set(LIB_DEPS
  cc_platform_netconfig_base
  cc_platform_netconfig_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  netconfig-grpc-provider.c++
  netconfig-providers-grpc.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
