## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_netconfig_protobuf)

set(LIB_DEPS
  cc_platform_netconfig_base
  cc_platform_proto
  cc_core_io_protobuf
)

set(SOURCES
  protobuf-netconfig-types.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
