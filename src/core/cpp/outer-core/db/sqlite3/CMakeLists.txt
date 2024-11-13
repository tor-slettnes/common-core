## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build a SQLite3 client wrapper
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_sqlite3)

### Package dependencies
set(PKG_DEPS
  sqlite3
)

### Library dependencies.
set(LIB_DEPS
  cc_core_sqlbase
)

### Source files in this folder
set(SOURCES
  sqlite3.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC
  USE_SQLITE3=1)
