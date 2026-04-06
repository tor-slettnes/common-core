## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Support for Confluent Schema Registry
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_schemaregistry)

set(LIB_DEPS
  cc_core_messaging_rest
)

set(SOURCES
  schema-registry-client.c++
)

cc_add_library(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
)


