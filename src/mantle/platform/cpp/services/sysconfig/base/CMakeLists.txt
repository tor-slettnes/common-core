## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core SysConfig service - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_sysconfig_base)

set(LIB_DEPS
  cc_core_common
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_platform_proto
)

set(SOURCES
  sysconfig-time.c++
  sysconfig-timezone.c++
  sysconfig-host.c++
  sysconfig-product.c++
  sysconfig-process.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
