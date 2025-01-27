## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build shared ZeroMQ wrappers
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_zmq_base)

### Package dependencies
# FetchContent_MakeAvailable(libzmq)
set(PKG_DEPS
  libzmq
)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_base
  cc_core_common
)

### Source files in this folder
set(SOURCES
  zmq-endpoint.c++
  zmq-host.c++
  zmq-satellite.c++
  zmq-filter.c++
  zmq-publisher.c++
  zmq-subscriber.c++
  zmq-messagehandler.c++
  zmq-messagewriter.c++
  zmq-requester.c++
  zmq-responder.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_ZMQ=1)

## Experimental interface ahead.
# cc_add_external_dependency(${TARGET} libzmq INCLUDE_DIRS LIBRARIES)
# cc_add_external_dependency(${TARGET} cppzmq INCLUDE_DIRS)
