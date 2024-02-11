## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_shared_ipc_http)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_ipc_common
)

### Third-party package dependencies
set(PKG_DEPS
  python3-embed
)

### Source files in this folder
set(SOURCES
)

### Recepies to build the library
include(BuildLibrary)

