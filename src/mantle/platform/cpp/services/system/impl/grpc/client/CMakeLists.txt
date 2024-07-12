## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core applications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream
### libraries/executables.
set(TARGET "cc_platform_system_grpc_client")

### Static/shared library dependencies, either from this build or provided by the system.
set(LIB_DEPS
  cc_platform_system_base
  cc_platform_system_grpc_common
  cc_core_messaging_grpc_client
)

### Source files
set(SOURCES
  system-grpc-time.c++
  system-grpc-host.c++
  system-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
