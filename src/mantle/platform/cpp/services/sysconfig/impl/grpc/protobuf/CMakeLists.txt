## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build ProtoBuf encoder/decoder methods for system service types
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET "cc_platform_sysconfig_protobuf")

### Static/shared library dependencies, either from this build or provided by the sysconfig.
set(LIB_DEPS
  cc_platform_sysconfig_base
  cc_core_io_protobuf
)

### Source files
set(SOURCES
  protobuf-sysconfig-types.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
