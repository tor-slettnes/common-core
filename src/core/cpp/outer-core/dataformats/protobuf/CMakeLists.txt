## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for shared types
## @author Tor Slettnes
#===============================================================================

set(TARGET "cc_core_io_protobuf")

### CMake module dependencies
set(MOD_DEPS
  Protobuf
)

### Native library dependencies.
set(LIB_DEPS
  cc_core_common
  protobuf::libprotobuf
)

### ProtoBuf dependencies
set(PROTO_DEPS
  cc_core_proto
)

set(SOURCES
  protobuf-standard-types.c++
  protobuf-variant-types.c++
  protobuf-event-types.c++
  protobuf-enum.c++
  protobuf-datetime-types.c++
  protobuf-version.c++
  protobuf-quantities.c++
  protobuf-message.c++
  protobuf-message-decoder.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  MOD_DEPS ${MOD_DEPS}
  SOURCES ${SOURCES}
)
