## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build service API library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_shared_ipc_api)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_core)

### Source files, roughly in dependency order
set(SOURCES
  ipc-endpoint.c++
)

## Invoke common CMake rules
include(BuildLibrary)
