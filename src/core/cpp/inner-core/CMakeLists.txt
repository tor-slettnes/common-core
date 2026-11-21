## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

add_subdirectory(common)
add_subdirectory(platforms)
add_subdirectory(tools)

if(BUILD_TESTING)
  add_subdirectory(testing)
endif()
