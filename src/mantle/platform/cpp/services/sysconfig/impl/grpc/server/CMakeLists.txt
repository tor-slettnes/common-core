## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
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

BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
