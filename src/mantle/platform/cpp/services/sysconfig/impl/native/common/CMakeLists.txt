## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build Common Core SysConfig service - native implementation
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_platform_sysconfig_native_common)

set(LIB_DEPS
  cc_platform_sysconfig_base
)

set(SOURCES
  sysconfig-native-time.c++
  sysconfig-native-host.c++
  sysconfig-native-product.c++
  sysconfig-native-process.c++
  sysconfig-providers-native-common.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
