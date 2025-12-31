## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

add_subdirectory(inner-core)
add_subdirectory(outer-core)

cc_add_doxygen(c++-reference
  FILE_PATTERNS "*.h++;*.md"
  EXTRACT_LOCAL_CLASSES NO)

