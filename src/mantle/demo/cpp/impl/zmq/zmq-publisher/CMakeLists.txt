## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Demo Application - ZeroMQ Publisher module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_zmq_publisher)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_zmq_common
)

### Source files
set(SOURCES
  demo-zmq-publisher.c++
  demo-zmq-signalwriter.c++
)

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
