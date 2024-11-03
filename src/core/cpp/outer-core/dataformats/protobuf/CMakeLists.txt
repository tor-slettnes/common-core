## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for shared types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_core_io_protobuf")

### CMake module dependencies
set(MOD_DEPS
  Protobuf
)

### Library dependencies.
set(LIB_DEPS
  cc_core_common
  cc_core_proto
  protobuf::libprotobuf
)

set(SOURCES
  protobuf-standard-types.c++
  protobuf-variant-types.c++
  protobuf-event-types.c++
  protobuf-datetime-types.c++
  protobuf-version.c++
  protobuf-message.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  MOD_DEPS ${MOD_DEPS}
  SOURCES ${SOURCES}
)
