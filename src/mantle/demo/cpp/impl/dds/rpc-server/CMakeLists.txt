## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Demo DDS service
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_dds_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_dds_types
  cc_demo_dds_publisher
  cc_core_messaging_dds_rti
)

### Source files
set(SOURCES
  demo-dds-requesthandler.c++
  demo-dds-service.c++
  demo-dds-run.c++
)

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
