## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET "cc_platform_sysconfig_grpc_client")

set(LIB_DEPS
  cc_platform_sysconfig_base
  cc_platform_sysconfig_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  sysconfig-grpc-time.c++
  sysconfig-grpc-timezone.c++
  sysconfig-grpc-host.c++
  sysconfig-grpc-product.c++
  sysconfig-grpc-process.c++
  sysconfig-providers-grpc.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
