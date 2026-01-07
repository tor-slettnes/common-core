## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Message Relay - ZeroMQ common declarations
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_relay_zmq_common)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_relay_common
  cc_core_messaging_zmq
)


cc_add_library("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)
