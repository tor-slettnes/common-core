## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard DDS client
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_switchboard_dds_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_switchboard_dds_subscriber)

### Source files
set(SOURCES
  remote-dds-switch.c++
  switchboard-dds-proxy.c++
  )

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
