## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to buld Relay ZMQ Broadcaster
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_relay_zmq_publisher)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_relay_zmq_common
  cc_core_messaging_zmq_base
)

### Source files
set(SOURCES
  relay-zmq-publisher.c++
  relay-zmq-signalwriter.c++
)

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
