## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_platform_vfs_protobuf)

set(LIB_DEPS
  cc_platform_vfs_base
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_platform_proto
)

set(SOURCES
  protobuf-vfs-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
