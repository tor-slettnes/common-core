## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildExecutable)
include(BuildLibrary)
include(BuildPython)

if (BUILD_DEMO)
  add_subdirectory(demo)
endif()

