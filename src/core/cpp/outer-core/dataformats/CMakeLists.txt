## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

if(BUILD_DDS)
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
