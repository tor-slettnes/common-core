## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_IDL)
  add_subdirectory(omg)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(protobuf)
endif()

if(BUILD_AVRO)
  add_subdirectory(avro)
endif()

if(BUILD_PYTHON_IO)
  add_subdirectory(python)
endif()
