## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_ZMQ)
  ### Name of this library.
  set(TARGET   cc_shared_ipc_zmq)
  set(LIB_TYPE INTERFACE)

  add_subdirectory(zmq-base)
  set(LIB_DEPS cc_shared_ipc_zmq_base)

  if(BUILD_PROTOBUF)
    add_subdirectory(zmq-protobuf)
    list(APPEND LIB_DEPS cc_shared_ipc_zmq_protobuf)
  endif()

  # if(BUILD_PYPICKLE)
  #   add_subdirectory(zmq-pypickle)
  #   list(APPEND LIB_DEPS cc_shared_ipc_zmq_pypickle)
  # endif()

  include(BuildLibrary)
endif()
