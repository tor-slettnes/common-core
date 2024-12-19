## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_kafka_producer)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_kafka_common
)

### Source files in this folder
set(SOURCES
  kafka-producer.c++
)

### Recepies to build the librar
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
