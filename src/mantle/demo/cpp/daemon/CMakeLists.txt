## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build service daemon for Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this executable. This also becomes the name of the project.
set(TARGET demoserver)

### Destination directory, if other than `bin`.
set(DEST_DIR sbin)

### Libraries we depend on, either from this build or provided by the
### system. Only direct dependencies are needed; e.g., do not list "cc_core"
### if you also list another library that depends on it.
set(LIB_DEPS
  cc_demo_native
)

if(BUILD_RTI_DDS)
  list(APPEND LIB_DEPS cc_demo_dds_service)
endif()

if(BUILD_GRPC)
  list(APPEND LIB_DEPS cc_demo_grpc_service)
endif()

if(BUILD_ZMQ)
  list(APPEND LIB_DEPS cc_demo_zmq_service)
endif()

### Source files
set(SOURCES
  main.c++
  options.c++
  )

## Invoke common CMake rules to build executable
include(BuildExecutable)
