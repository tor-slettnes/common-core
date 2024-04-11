## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Common Core Demo Application - ZeroMQ Subscriber module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_zmq_subscriber)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_zmq_common
)

### Source files
set(SOURCES
  demo-zmq-subscriber.c++
  demo-zmq-signalhandler.c++
)

### Invoke common CMake rules
build_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
