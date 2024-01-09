##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (USE_CPP)
  add_subdirectory(cpp)
endif()

if (USE_PYTHON)
  add_subdirectory(python)
endif()

if (USE_GRPC)
  add_subdirectory(protodefs)
endif()

if (USE_RTI_DDS)
  add_subdirectory(idl)
endif()

### Install "settings/" folder as data
install(DIRECTORY settings TYPE DATA)
