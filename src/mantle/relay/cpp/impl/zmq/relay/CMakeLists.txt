## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Relay Application - ZeroMQ server module
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_relay_zmq_run)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_relay_zmq_publisher
  cc_relay_zmq_subscriber
)

### Source files
set(SOURCES
  relay-zmq-run.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
