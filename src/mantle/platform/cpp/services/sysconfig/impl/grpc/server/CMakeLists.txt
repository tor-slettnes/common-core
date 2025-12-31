## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET "cc_platform_sysconfig_grpc_service")

set(LIB_DEPS
  cc_platform_sysconfig_base
  cc_platform_sysconfig_protobuf
  cc_core_messaging_grpc_server
)

set(SOURCES
  sysconfig-grpc-requesthandler.c++
  sysconfig-grpc-signalqueue.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
