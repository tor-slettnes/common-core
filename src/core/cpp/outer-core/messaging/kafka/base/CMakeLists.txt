## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_messaging_kafka_base)

set(PKG_DEPS
   rdkafka++
)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_base
)

set(SOURCES
  kafka-callbacks.c++
  kafka-endpoint.c++
  kafka-producer.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_KAFKA=1)
