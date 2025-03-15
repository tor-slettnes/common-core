## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo application
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(api)
add_subdirectory(impl)

if(BUILD_DEMO)
  add_subdirectory(daemon)
  add_subdirectory(utils)

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()

