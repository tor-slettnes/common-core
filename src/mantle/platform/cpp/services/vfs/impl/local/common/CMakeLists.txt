## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_vfs_local_common)

set(LIB_DEPS
  cc_platform_vfs_base
)

set(SOURCES
  vfs-attribute-store.c++
  vfs-local-context.c++
  vfs-local-fileio.c++
  vfs-local-provider.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
