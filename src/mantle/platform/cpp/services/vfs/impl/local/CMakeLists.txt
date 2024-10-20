## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### OS agnostic VFS providers
add_subdirectory(common)

### Linux-specific VFS providers
if(LINUX)
  add_subdirectory(posix)
  add_subdirectory(linux)

  set(LIB_DEPS
    cc_platform_vfs_local_linux
  )
endif()

### Build container library
set(TARGET
  cc_platform_vfs_local
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
)
