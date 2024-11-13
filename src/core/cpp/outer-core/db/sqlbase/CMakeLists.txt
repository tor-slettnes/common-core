## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build a SQL language support
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_sqlbase)

### Library dependencies.
set(LIB_DEPS
  cc_core_inner
)

### Source files in this folder
set(SOURCES
  sqlcommand.c++
  sqlbase.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
