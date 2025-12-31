## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes
#===============================================================================

add_subdirectory(main)

if(BUILD_SHARED_APPS)
  add_subdirectory(suites)
endif()
