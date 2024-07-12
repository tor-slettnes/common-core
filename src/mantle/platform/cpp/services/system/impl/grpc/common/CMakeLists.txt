## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for system service types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET "cc_platform_system_grpc_common")

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_platform_system_base
  cc_core_io_protobuf
)

### Source files
set(SOURCES
  protobuf-system-types.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
