## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard service implementations
## @author Tor Slettnes
#===============================================================================

add_subdirectory(native)

if(BUILD_DDS)
  add_subdirectory(dds)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()

