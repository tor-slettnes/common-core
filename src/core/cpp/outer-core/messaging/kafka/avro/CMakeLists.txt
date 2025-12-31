## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_kafka_avro)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_kafka_base
  cc_core_io_avro_base
)

### Source files in this folder
set(SOURCES
  kafka-producer-avro.c++
)

### Recepies to build the librar
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
