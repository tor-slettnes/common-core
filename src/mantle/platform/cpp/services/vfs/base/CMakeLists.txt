## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_vfs_base)

set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  vfs-types.c++
  vfs-context.c++
  vfs-signals.c++
  vfs-base.c++
  vfs.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS "${LIB_DEPS}"
  SOURCES "${SOURCES}"
)
