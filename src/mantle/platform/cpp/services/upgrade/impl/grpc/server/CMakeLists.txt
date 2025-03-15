## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_upgrade_grpc_service")

set(LIB_DEPS
  cc_platform_upgrade_base
  cc_platform_upgrade_protobuf
  cc_core_messaging_grpc_server
)

set(SOURCES
  upgrade-grpc-signalqueue.c++
  upgrade-grpc-requesthandler.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
