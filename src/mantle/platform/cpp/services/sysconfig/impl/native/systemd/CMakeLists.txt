## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build Common Core SysConfig service - SystemD implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_sysconfig_native_systemd)

set(LIB_DEPS
  cc_platform_sysconfig_native_posix
)

set(SOURCES
  sysconfig-providers-native-systemd.c++
  sysconfig-systemd-time.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
