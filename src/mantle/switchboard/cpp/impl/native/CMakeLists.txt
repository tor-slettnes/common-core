## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build atandalone switchboard, mainly for server
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_switchboard_native)

set(LIB_DEPS
  cc_switchboard_base
)

set(SOURCES
  local-switch.c++
  switchboard-central.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
