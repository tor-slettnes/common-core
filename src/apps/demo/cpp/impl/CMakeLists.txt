##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Demo application - implementations
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(native)

if(BUILD_RTI_DDS)
  add_subdirectory(dds)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()

