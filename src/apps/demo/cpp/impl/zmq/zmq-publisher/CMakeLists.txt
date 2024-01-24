## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Common Core Demo Application - ZeroMQ Publisher module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_zmq_publisher)

### What kind of library we are building (STATIC|SHARED|OBJECT|...); the default
### is STATIC.  See:
### https://cmake.org/cmake/help/latest/command/add_library.html.
set(LIB_TYPE STATIC)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_zmq_common
)

### Source files
set(SOURCES
  demo-zmq-publisher.c++
)

### Invoke common CMake rules
include(BuildLibrary)
