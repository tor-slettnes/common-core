## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build shared ZeroMQ wrappers
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_zmq_base)

### Library dependencies.
set(LIB_TYPE STATIC)

###FetchContent_MakeAvailable(libzmq)
### Package dependencies
set(PKG_DEPS
  libzmq)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_common
  cc_core_common
  zmq
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
include(BuildLibrary)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_ZMQ=1)
