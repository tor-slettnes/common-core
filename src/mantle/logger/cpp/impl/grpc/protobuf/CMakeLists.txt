## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for logger types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET "cc_logger_protobuf")

### Static/shared library dependencies, either from this build or provided by the sysconfig.
set(LIB_DEPS
  cc_logger_base
  cc_logger_proto
  cc_core_io_protobuf
)

### Source files
set(SOURCES
  protobuf-logger-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
