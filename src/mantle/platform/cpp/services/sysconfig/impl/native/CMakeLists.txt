## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build Common Core SysConfig service - native implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### OS agnostic SysConfig providers
add_subdirectory(common)

### Linux specific SysConfig providers
if(LINUX)
  add_subdirectory(posix)
  add_subdirectory(linux)
  add_subdirectory(systemd)

  set(LIB_DEPS
    cc_platform_sysconfig_native_linux
    cc_platform_sysconfig_native_systemd
  )
endif()

### Build container library
set(TARGET cc_platform_sysconfig_native)

set(SOURCES
  sysconfig-providers-native.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
