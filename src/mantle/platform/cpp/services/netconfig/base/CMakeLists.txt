## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_netconfig_base)

set(LIB_DEPS
  cc_core_common
)

set(PKG_DEPS
  libnm
)

set(SOURCES
  netconfig-types.c++
  netconfig-signals.c++
  netconfig.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS "${LIB_DEPS}"
  PKG_DEPS "${PKG_DEPS}"
  SOURCES "${SOURCES}"
)
