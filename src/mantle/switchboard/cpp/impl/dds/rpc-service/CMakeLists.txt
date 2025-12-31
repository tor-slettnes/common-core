## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard DDS service
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_switchboard_dds_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_switchboard_base
  cc_switchboard_dds_types
  cc_switchboard_dds_publisher
  cc_core_messaging_dds_rti
)

### Source files
set(SOURCES
  switchboard-dds-requesthandler.c++
  switchboard-dds-service.c++
)

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
