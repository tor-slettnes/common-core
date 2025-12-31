## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Demo Application - ZeroMQ client module
## @author Tor Slettnes
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_multilogger_zmq_client)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_multilogger_zmq_common
)

### Source files
set(SOURCES
  multilogger-zmq-client.c++
  multilogger-zmq-client-sink.c++
  multilogger-zmq-client-reader.c++
  multilogger-zmq-client-writer.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
