## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_DEMO)
  add_subdirectory(demo)
endif()

add_subdirectory(platform)
