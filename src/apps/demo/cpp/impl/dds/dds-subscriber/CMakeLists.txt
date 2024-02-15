## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Common Core Demo Application - DDS Subscriber module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_dds_subscriber)

### What kind of library we are building (STATIC|SHARED|OBJECT|...); the default
### is STATIC.  See:
### https://cmake.org/cmake/help/latest/command/add_library.html.
set(LIB_TYPE STATIC)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_dds_types
  cc_shared_messaging_dds_rti
)

### Source files
set(SOURCES
  demo-dds-subscriber.c++)

### Invoke common CMake rules
include(BuildLibrary)
