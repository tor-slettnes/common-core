## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard DDS pubilsher
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_switchboard_dds_publisher)

### Library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_switchboard_base
  cc_switchboard_dds_types
  cc_core_messaging_dds_rti)

### Source files
set(SOURCES
  switchboard-dds-publisher.c++)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
