## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(services)

if(BUILD_PLATFORM)
  add_subdirectory(daemon)
  add_subdirectory(tools)
endif()

