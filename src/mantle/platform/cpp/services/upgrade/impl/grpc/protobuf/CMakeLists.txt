## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_upgrade_protobuf)

set(LIB_DEPS
  cc_platform_upgrade_base
  cc_platform_sysconfig_protobuf
  cc_platform_vfs_protobuf
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_platform_proto
)

set(SOURCES
  protobuf-upgrade-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
