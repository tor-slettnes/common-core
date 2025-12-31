## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

add_subdirectory(common)

if(BUILD_SQLITE3)
  add_subdirectory(sqlite3)
endif()
