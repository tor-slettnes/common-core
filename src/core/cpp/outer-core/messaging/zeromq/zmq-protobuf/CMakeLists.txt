## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_zmq_protobuf)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_zmq_base
  cc_core_io_protobuf
)

### Source files in this folder
set(SOURCES
  zmq-protobuf-error.c++
  zmq-protobuf-client.c++
  zmq-protobuf-requesthandler.c++
  zmq-protobuf-server.c++
)

### Recepies to build the librar
BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
