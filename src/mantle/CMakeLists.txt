## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DEMO)
  add_subdirectory(demo)
else()
  add_subdirectory(demo EXCLUDE_FROM_ALL)
endif()

if(BUILD_PLATFORM)
  add_subdirectory(platform)
else()
  add_subdirectory(platform EXCLUDE_FROM_ALL)
endif()

if(BUILD_LOGGER)
  add_subdirectory(multilogger)
else()
  add_subdirectory(multilogger EXCLUDE_FROM_ALL)
endif()
