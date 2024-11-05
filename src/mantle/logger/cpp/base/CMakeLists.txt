## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_logger_base)

### Make this an OBJECT library whose files will be included direcly in dowstream dependants
set(LIB_TYPE STATIC)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_core
)

set(SOURCES
  logger-types.c++
  logger-api.c++
)

cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
