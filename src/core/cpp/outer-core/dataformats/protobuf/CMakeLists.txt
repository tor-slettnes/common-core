## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for shared types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_core_io_protobuf)

### CMake module dependencies
# set(MOD_DEPS
#   Protobuf
# )

set(PKG_DEPS
  protobuf
)


### Library dependencies.
set(LIB_DEPS
  cc_core_common
  cc_core_proto
  # protobuf::libprotobuf
)

set(SOURCES
  protobuf-standard-types.c++
  protobuf-variant-types.c++
  protobuf-event-types.c++
  protobuf-version.c++
  protobuf-message.c++
)

### Recepies to build the library
BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)
