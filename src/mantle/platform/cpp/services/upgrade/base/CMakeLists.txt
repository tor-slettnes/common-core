## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_upgrade_base)

set(LIB_DEPS
  cc_platform_sysconfig_base
  cc_platform_vfs_base
)

set(SOURCES
  upgrade-types.c++
  upgrade-signals.c++
  upgrade-base.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS "${LIB_DEPS}"
  SOURCES "${SOURCES}"
)
