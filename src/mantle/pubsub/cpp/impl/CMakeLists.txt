## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Pub/Sub Implementations
## @author Tor Slettnes
#===============================================================================

# add_subdirectory(broker)

if(BUILD_ZMQ)
  add_subdirectory(zmq)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()

# if(BUILD_WEBSOCKETS)
#   add_subdirectory(ws)
# endif()
