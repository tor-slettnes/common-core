## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Common Core Demo Application - ZeroMQ Subscriber module
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET cc_multilogger_zmq_server)

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_multilogger_zmq_common
)

### Source files
set(SOURCES
  multilogger-zmq-run.c++
  multilogger-zmq-server.c++
  multilogger-zmq-request-handler.c++
  multilogger-zmq-submission-handler.c++
  multilogger-zmq-message-writer.c++
)

### Invoke common CMake rules
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
