## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build a SQLite3 log sink
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_multilogger_sink_sqlite3)

### Library dependencies.
set(LIB_DEPS
  cc_core_sqlite3
)

### Source files in this folder
set(SOURCES
  multilogger-sqlite3-sink.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_SQLITE3=1)
