## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(sinks)
add_subdirectory(base)
add_subdirectory(impl)

if(BUILD_LOGGER)
  add_subdirectory(daemon)
  add_subdirectory(tools)
endif()
