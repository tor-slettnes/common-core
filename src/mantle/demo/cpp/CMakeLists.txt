## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo application
## @author Tor Slettnes
#===============================================================================

add_subdirectory(api)
add_subdirectory(impl)

if(BUILD_DEMO)
  add_subdirectory(daemon)
  add_subdirectory(tools)

  if(BUILD_DEMO AND BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()
