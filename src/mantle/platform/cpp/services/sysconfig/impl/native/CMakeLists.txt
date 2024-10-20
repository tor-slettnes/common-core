## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build Common Core SysConfig service - native implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### OS agnostic SysConfig providers
add_subdirectory(common)

### Linux specific SysConfig providers
if(LINUX)
  add_subdirectory(posix)
  add_subdirectory(linux)

  set(LIB_DEPS
    cc_platform_sysconfig_native_linux
  )
endif()

### Build container library
set(TARGET cc_platform_sysconfig_native)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
)
