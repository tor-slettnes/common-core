## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build Common Core SysConfig service - native Linux implementation
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_platform_sysconfig_native_linux)

set(LIB_DEPS
  cc_platform_sysconfig_native_posix
)

set(SOURCES
  sysconfig-providers-native-linux.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
