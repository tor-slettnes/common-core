## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build a SQLite3 log sink
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if (BUILD_SQLITE3)
  add_subdirectory(sqlite3)
endif()
