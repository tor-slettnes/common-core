## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core System service - abstract API
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_platform_system_base)

### Make this an OBJECT library whose files will be included direcly in dowstream dependants
set(LIB_TYPE STATIC)

### Static/shared library dependencies.
set(LIB_DEPS
  cc_platform_proto
  cc_core_common
  cc_core_io_protobuf
)

set(SOURCES
  system-api-time.c++
  system-api-timezone.c++
  system-api-host.c++
  system-api-product.c++
  system-api-process.c++
)

BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
