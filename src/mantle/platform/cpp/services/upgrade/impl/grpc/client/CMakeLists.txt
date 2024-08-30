## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_upgrade_grpc_client")

set(LIB_DEPS
  cc_platform_upgrade_base
  cc_platform_upgrade_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  upgrade-grpc-client.c++
  upgrade-grpc-provider.c++
  upgrade-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
