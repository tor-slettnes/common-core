## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Abstract service interface, agnostic to communications bus
add_subdirectory(common)

if(BUILD_DDS)
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

if(BUILD_DBUS)
  add_subdirectory(dbus)
endif()
