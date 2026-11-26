## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

if(BUILD_DBUS)
  add_subdirectory(dbus)
endif()

# if(BUILD_UDEV)
#   add_subdirectory(udev)
# endif()
