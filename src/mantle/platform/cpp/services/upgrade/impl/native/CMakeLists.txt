## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_upgrade_native)

set(LIB_DEPS
  cc_platform_upgrade_base
)

set(SOURCES
  upgrade-native-provider.c++
  package-manifest.c++
  package-handler.c++
  package-handler-vfs.c++
  package-handler-http.c++
  upgrade-providers-native.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
