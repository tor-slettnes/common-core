## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build unit tests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(main)

if(BUILD_SHARED)
  add_subdirectory(suites)
endif()
