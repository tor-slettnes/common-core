## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build a SQL base wrapper
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_sql_common)

### Library dependencies.
set(LIB_DEPS
  cc_core_common
)

### Source files in this folder
set(SOURCES
  sql-common.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)

