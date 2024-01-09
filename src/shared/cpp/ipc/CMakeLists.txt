##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Abstract service interface, agnostic to communications bus
add_subdirectory(api)

### RTI DDS support
if (USE_RTI_DDS)
  add_subdirectory(rti)
endif()

### GRPC support
if (USE_GRPC)
  add_subdirectory(google)
endif()
