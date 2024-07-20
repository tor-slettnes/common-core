## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_network_grpc_client")

set(LIB_DEPS
  cc_platform_network_base
  cc_platform_network_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  network-grpc-provider.c++
  network-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
