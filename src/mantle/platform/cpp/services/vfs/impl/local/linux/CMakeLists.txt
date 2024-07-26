## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_vfs_local_linux)

set(LIB_DEPS
  cc_platform_vfs_local_posix
)

set(PKG_DEPS
  libudev
)

set(SOURCES
  vfs-linux-provider.c++
  vfs-linux-volmon.c++
  vfs-linux-removable-context.c++
  vfs-providers-linux.c++
)

BuildLibrary("${TARGET}"
  PKG_DEPS ${PKG_DEPS}
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
