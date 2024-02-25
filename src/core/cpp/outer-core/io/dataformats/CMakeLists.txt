## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DDS)
  add_subdirectory(omg)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(protobuf)
endif()

if(BUILD_PYTHON_IO)
  message(STATUS "ADDING SUBDIRECTORY 'python'")
  add_subdirectory(python)
else()
  message(STATUS "NOT ADDING SUBDIRECTORY 'python'")
endif()


