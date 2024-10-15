## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(api EXCLUDE_FROM_ALL)
add_subdirectory(impl EXCLUDE_FROM_ALL)

if(INCLUDE_DEMO)
  add_subdirectory(daemon)
  add_subdirectory(utils)

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()

