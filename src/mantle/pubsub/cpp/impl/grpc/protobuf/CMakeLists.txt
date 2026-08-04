## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_relay_protobuf)

set(LIB_DEPS
  cc_relay_common
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_pubsub_proto
)

set(SOURCES
  protobuf-relay-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
