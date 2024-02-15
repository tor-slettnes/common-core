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
  # curlpp
  libcurl
)

### Source files in this folder
set(SOURCES
  http-client.c++
)

### Recepies to build the library
include(BuildLibrary)

