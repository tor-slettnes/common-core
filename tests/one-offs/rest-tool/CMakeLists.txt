## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

if (BUILD_CPP)
  add_subdirectory(cpp)
endif()
