## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_vfs_local_posix)

set(LIB_DEPS
  cc_platform_vfs_local_common
)

set(SOURCES
  vfs-posix-provider.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
