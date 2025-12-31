## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build switchboard abstract interface
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_switchboard_base)

set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  switch.c++
  switch-dependency.c++
  switch-interceptor.c++
  switchboard-types.c++
  switchboard-provider.c++
  switchboard-signals.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
