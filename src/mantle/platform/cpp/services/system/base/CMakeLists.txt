## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core System service - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_system_base)

set(LIB_DEPS
  cc_platform_proto
  cc_core_common
  cc_core_io_protobuf
)

set(SOURCES
  system-provider-time.c++
  system-provider-timezone.c++
  system-provider-host.c++
  system-provider-product.c++
  system-provider-process.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
