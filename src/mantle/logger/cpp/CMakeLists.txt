## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(services)

if(INCLUDE_LOGGER)
  add_subdirectory(daemon)
  add_subdirectory(tools)
endif()
