## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

add_subdirectory(common)
add_subdirectory(platforms)

if(BUILD_TESTING)
  add_subdirectory(testing)
endif()
