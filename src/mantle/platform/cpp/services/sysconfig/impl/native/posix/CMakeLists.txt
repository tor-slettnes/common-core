## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build Common Core SysConfig service - native POSIX implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_sysconfig_native_posix)

set(LIB_DEPS
  cc_platform_sysconfig_native_common
)

set(SOURCES
  sysconfig-posix-timezone.c++
  sysconfig-providers-native-posix.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
