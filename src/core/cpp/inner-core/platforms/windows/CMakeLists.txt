## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build platform-specific code - Windows version
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_core_platform_windows)

### Library dependencies
set(LIB_DEPS
  cc_core_common
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
BuildLibrary("${TARGET}"
  LIB_TYPE STATIC
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)


### Allow dynamic linking
set_target_properties(${TARGET} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
