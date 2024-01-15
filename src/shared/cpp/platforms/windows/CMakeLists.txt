## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build platform-specific code - POSIX version
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_shared_platform_windows)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies
set(LIB_DEPS
  cc_shared_core
)

### Sources
set(SOURCES
  windows-providers.c++
  windows-symbols.c++
  windows-path.c++
  windows-process.c++
  windows-runtime.c++
  windows-timezone.c++
  windows-logsink.c++
)

### Build library
include(BuildLibrary)

### Allow dynamic linking
set_target_properties(${TARGET} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
