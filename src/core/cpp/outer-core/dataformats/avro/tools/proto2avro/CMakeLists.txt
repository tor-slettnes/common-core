## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Proto2Avro tool
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET proto2avro)

### Libraries we depend on, either from this build or provided by the
### system.
set(LIB_DEPS
  cc_core_io_avro_protobuf
  cc_core_platform
)

### Source files
set(SOURCES
  main.c++
  )

## Invoke common CMake rules to build executable
cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)
