## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description Build Common Core SysConfig service - native Linux implementation
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_sysconfig_native_linux)

set(LIB_DEPS
  cc_platform_sysconfig_native_posix
)

set(SOURCES
  sysconfig-providers-native-linux.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
