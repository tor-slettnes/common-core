## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()

if (BUILD_PYTHON)
  add_subdirectory(python)
endif()

if (BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if (BUILD_RTI_DDS)
  add_subdirectory(idl)
endif()

### Install "settings/" folder as data
install(DIRECTORY settings TYPE DATA)
