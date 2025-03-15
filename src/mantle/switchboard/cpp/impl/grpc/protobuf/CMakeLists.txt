## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_switchboard_protobuf)

set(LIB_DEPS
  cc_switchboard_base
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_switchboard_proto
)

set(SOURCES
  protobuf-switchboard-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
