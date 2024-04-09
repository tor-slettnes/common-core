## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for shared types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_core_io_protobuf)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_core_common
  cc_core_proto
  protobuf
)

set(SOURCES
  protobuf-standard-types.c++
  protobuf-variant-types.c++
  protobuf-event-types.c++
  protobuf-message.c++
)

### Recepies to build the library
include(BuildLibrary)


if(TARGET protobuf::libprotobuf)
  target_link_directories(${TARGET} PUBLIC
    "$<TARGET_PROPERTY:protobuf::libprotobuf,BINARY_DIR>"
  )
endif()

