## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

add_subdirectory(base)
add_subdirectory(protobuf)
add_subdirectory(tools)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()

