## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (INCLUDE_DEMO)
  add_subdirectory(demo)
else()
  add_subdirectory(demo EXCLUDE_FROM_ALL)
endif()

if (INCLUDE_PLATFORM)
  add_subdirectory(platform)
else()
  add_subdirectory(platform EXCLUDE_FROM_ALL)
endif()

if (INCLUDE_LOGGER)
  add_subdirectory(logger)
else()
  add_subdirectory(logger EXCLUDE_FROM_ALL)
endif()
