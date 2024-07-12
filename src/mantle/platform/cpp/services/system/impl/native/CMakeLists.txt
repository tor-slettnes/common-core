## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core System service - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_platform_system_native)

### Make this an OBJECT library whose files will be included direcly in dowstream dependants
set(LIB_TYPE STATIC)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_platform_system_base
)

set(SOURCES
  system-native-time.c++
  system-native-host.c++
  system-providers-native.c++
)

BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
