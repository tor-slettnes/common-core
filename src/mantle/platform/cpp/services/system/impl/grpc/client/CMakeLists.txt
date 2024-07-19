## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_system_grpc_client")

set(LIB_DEPS
  cc_platform_system_base
  cc_platform_system_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  system-grpc-time.c++
  system-grpc-host.c++
  system-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
