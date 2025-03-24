## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(base)
add_subdirectory(impl)

if(BUILD_SWITCHBOARD)
  add_subdirectory(daemon)
  add_subdirectory(utils)
endif()
