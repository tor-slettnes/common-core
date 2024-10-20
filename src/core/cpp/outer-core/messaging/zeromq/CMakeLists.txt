## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_ZMQ)
  ### Name of this library.
  set(TARGET   cc_core_messaging_zmq)

  add_subdirectory(zmq-base)
  set(LIB_DEPS cc_core_messaging_zmq_base)

  if(BUILD_PROTOBUF)
    add_subdirectory(zmq-protobuf)
    list(APPEND LIB_DEPS cc_core_messaging_zmq_protobuf)
  endif()

  if(BUILD_PYTHON_IO)
    add_subdirectory(zmq-pypickle)
    list(APPEND LIB_DEPS cc_core_messaging_zmq_pypickle)
  endif()

  cc_add_library("${TARGET}"
    LIB_TYPE INTERFACE
    LIB_DEPS ${LIB_DEPS}
  )
endif()
