## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core MultiLogger Application - ZeroMQ comon definitions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_multilogger_zmq_common)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_multilogger_base
  cc_multilogger_protobuf
  cc_core_messaging_zmq
  cc_core_io_protobuf
)


cc_add_library("${TARGET}"
  LIB_TYPE INTERFACE
  LIB_DEPS ${LIB_DEPS}
)
