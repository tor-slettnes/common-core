## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_io_avro_base)

set(PKG_DEPS
   avro-c
)

### Library dependencies.
set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  avro-schemabuilder.c++
  avro-basevalue.c++
  avro-compoundvalue.c++
  avro-simplevalue.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_AVRO=1)

