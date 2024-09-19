## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_platform_upgrade_native)

set(LIB_DEPS
  cc_platform_upgrade_base
  cc_core_messaging_http
)

set(SOURCES
  upgrade-native-provider.c++
  package-manifest.c++
  package-handler.c++
  package-handler-vfs.c++
  package-handler-url.c++
  package-index.c++
  package-index-vfs.c++
  package-index-url.c++
  upgrade-providers-native.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
