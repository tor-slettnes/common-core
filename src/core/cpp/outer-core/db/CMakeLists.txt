## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_SQLITE3)
  add_subdirectory(sqlite3)
endif()
