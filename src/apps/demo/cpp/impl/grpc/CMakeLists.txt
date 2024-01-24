## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Demo application interface - DDS implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_PROTOBUF)
  add_subdirectory(types)
endif()

if(BUILD_GRPC)
  add_subdirectory(server)
  add_subdirectory(client)
endif()

