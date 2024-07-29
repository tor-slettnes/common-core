## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core SysConfig service - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_sysconfig_base)

set(LIB_DEPS
  cc_platform_proto
  cc_core_common
  cc_core_io_protobuf
)

set(SOURCES
  sysconfig-time.c++
  sysconfig-timezone.c++
  sysconfig-host.c++
  sysconfig-product.c++
  sysconfig-process.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
