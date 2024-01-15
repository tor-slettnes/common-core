## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for shared types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_shared_ipc_protobuf)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_core
  cc_shared_protodefs
)

set(SOURCES
  translate-protobuf-google.c++
  translate-protobuf-variant.c++
  translate-protobuf-event.c++
  protobuf-message.c++
)

### Recepies to build the library
include(BuildLibrary)
