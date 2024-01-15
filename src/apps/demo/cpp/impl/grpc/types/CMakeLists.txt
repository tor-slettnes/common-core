## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for Demo types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_demo_protobuf)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protodefs
  cc_shared_ipc_protobuf
)

### External package dependencies, configured via `pkg-config`.
set(PKG_DEPS
  protobuf
)

set(SOURCES
  translate-protobuf-demo.c++
)

### Recepies to build the library
include(BuildLibrary)
