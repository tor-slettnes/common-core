## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

add_subdirectory(sqlbase)

if(BUILD_SQLITE3)
  add_subdirectory(sqlite3)
endif()
