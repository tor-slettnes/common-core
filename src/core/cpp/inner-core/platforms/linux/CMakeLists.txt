## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build platform-specific code - POSIX version
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_platform_linux)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies
set(LIB_DEPS
  cc_core_platform_posix
)

### External package dependencies
set(PKG_DEPS
  uuid)

### Sources
set(SOURCES
  linux-providers.c++
  linux-symbols.c++
  linux-path.c++
)

### Build library
include(BuildLibrary)
