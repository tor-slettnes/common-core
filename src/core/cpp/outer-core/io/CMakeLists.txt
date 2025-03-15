## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DBUS)
  add_subdirectory(dbus)
endif()
