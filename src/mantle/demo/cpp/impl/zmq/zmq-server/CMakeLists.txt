## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Common Core Demo Application - ZeroMQ server module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_zmq_service)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_zmq_publisher
)

### Source files
set(SOURCES
  demo-zmq-requesthandler.c++
  demo-zmq-server.c++
  demo-zmq-run.c++
)

cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
