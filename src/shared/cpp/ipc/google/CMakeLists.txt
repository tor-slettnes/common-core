## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_PROTOBUF)
  add_subdirectory(protobuf)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()
