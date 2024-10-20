## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build DDS RPC client library for demo application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_dds_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_dds_types
  cc_demo_dds_subscriber
  cc_core_messaging_dds_rti
)

### Source files
set(SOURCES
  demo-dds-client.c++
  )

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
