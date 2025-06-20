## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for multilogger types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET "cc_multilogger_protobuf")

### Static/shared library dependencies, either from this build or provided by the sysconfig.
set(LIB_DEPS
  cc_multilogger_base
  cc_core_io_protobuf
)

set(PROTO_DEPS
  cc_multilogger_proto
)

### Source files
set(SOURCES
  protobuf-multilogger-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
