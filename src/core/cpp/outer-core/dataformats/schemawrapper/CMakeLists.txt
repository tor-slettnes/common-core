## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description (De)serialization wrapper for confluent schema
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_schema_wrapper)

set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  schema-wrapper.c++
)

cc_add_library(${TARGET}
  SOURCES ${SOURCES}
  LIB_DEPS ${LIB_DEPS}
)

