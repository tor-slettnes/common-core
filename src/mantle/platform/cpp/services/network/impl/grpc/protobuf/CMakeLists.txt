## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_network_protobuf)

set(LIB_DEPS
  cc_platform_network_base
  cc_platform_proto
  cc_core_io_protobuf
)

set(SOURCES
  protobuf-network.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
