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

### Obtain common defaults
include(${CMAKE_CURRENT_LIST_DIR}/defaults.cmake)
