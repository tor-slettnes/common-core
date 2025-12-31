## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Demo Application - ZeroMQ client module
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_demo_zmq_common)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_demo_api
  cc_demo_protobuf_types
  cc_core_messaging_zmq
  cc_core_io_protobuf
)


cc_add_library("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)
