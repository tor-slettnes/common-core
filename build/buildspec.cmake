## -*- cmake -*-
#===============================================================================
## @file buildspec.cmake
## @brief Miscellanous build configuration
##
## To make local persistent changes, put your customizations in your own
## `local.cmake` file in this folder; as this that is listed in `.gitignore` it
## won't be committed.  For your custom setting to take effect you may need to
## use a plain `set()` invocation without the CACHE option, or add the `FORCE`
## option, or else clean the CMake cache before rebuilding.
#===============================================================================

### Obtain local overides to build configuration
include(${CMAKE_CURRENT_LIST_DIR}/local.cmake OPTIONAL)

### Set default version number from `version` file if not provided
file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/default_version default_version)
set(VERSION "${default_version}"
  CACHE STRING "Build version, in the format MAJOR.MINOR.PATCH")

### Obtain common defaults
include(${CMAKE_CURRENT_LIST_DIR}/defaults.cmake)

if(NOT BUILD_NUMBER)
  ### No BUILD_NUMBER argument was provided. Let's check the BUILD_NUMBER
  if(DEFINED ENV{BUILD_NUMBER})
    set(BUILD_NUMBER $ENV{BUILD_NUMBER})
  else()
    math(EXPR BUILD_NUMBER "${LAST_BUILD}+1")
  endif()
endif()

set(LAST_BUILD "${BUILD_NUMBER}" CACHE INTERNAL "..." FORCE)
