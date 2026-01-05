## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Demo Application - ZeroMQ proxy module
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_pubsub_zmq_proxy)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_core_messaging_zmq_base
)

### Source files
set(SOURCES
  pubsub-zmq-proxy.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
