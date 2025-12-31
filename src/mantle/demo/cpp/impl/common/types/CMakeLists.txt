## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for Demo types
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_demo_protobuf_types)

### Library dependencies.
set(LIB_DEPS
  cc_demo_api
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_demo_proto
)

set(SOURCES
  protobuf-demo-types.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
