## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Abstract service interface, agnostic to communications bus
add_subdirectory(base)

if(BUILD_IDL)
  add_subdirectory(dds)
endif()

if(BUILD_GRPC)
  add_subdirectory(grpc)
endif()

if(BUILD_ZMQ)
  add_subdirectory(zeromq)
endif()

if(BUILD_HTTP)
  add_subdirectory(http)
endif()

if(BUILD_KAFKA)
  add_subdirectory(kafka)
endif()
