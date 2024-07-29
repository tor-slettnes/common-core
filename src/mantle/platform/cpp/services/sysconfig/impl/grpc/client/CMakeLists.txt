## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_sysconfig_grpc_client")

set(LIB_DEPS
  cc_platform_sysconfig_base
  cc_platform_sysconfig_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  sysconfig-grpc-time.c++
  sysconfig-grpc-host.c++
  sysconfig-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
