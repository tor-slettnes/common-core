## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switch control tool
## @author Tor Slettnes
#===============================================================================

add_subdirectory(common)

if(BUILD_DDS)
  add_subdirectory(dds-tool)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc-tool)
endif()
