##
## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Include components common to all products
add_subdirectory(common)
add_subdirectory(demoproduct)

### Build common applications
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/CMakeLists.txt")
  add_subdirectory(${PROJECT_NAME})
endif()

