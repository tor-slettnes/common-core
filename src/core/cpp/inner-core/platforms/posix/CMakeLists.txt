## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build platform-specific code - POSIX version
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_platform_posix)

### Library dependencies
set(LIB_DEPS
  cc_core_common
)

### Sources
set(SOURCES
  posix-host.c++
  posix-logsink.c++
  posix-path.c++
  posix-process.c++
  posix-providers.c++
  posix-runtime.c++
  posix-serialport.c++
  posix-symbols.c++
  posix-timezone.c++
  posix-user.c++
)

### Build library
cc_add_library("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
