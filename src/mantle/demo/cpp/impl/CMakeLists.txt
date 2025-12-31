## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Demo application - implementations
## @author Tor Slettnes
#===============================================================================

add_subdirectory(common)
add_subdirectory(native)

if(BUILD_DDS)
  add_subdirectory(dds)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()

if(BUILD_ZMQ)
  add_subdirectory(zmq)
endif()

