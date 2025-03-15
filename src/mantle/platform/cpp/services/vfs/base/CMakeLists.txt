## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_vfs_base)

set(LIB_DEPS
  cc_core_common
)

set(SOURCES
  vfs-types.c++
  vfs-context.c++
  vfs-location.c++
  vfs-signals.c++
  vfs-base.c++
  vfs.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS "${LIB_DEPS}"
  SOURCES "${SOURCES}"
)
