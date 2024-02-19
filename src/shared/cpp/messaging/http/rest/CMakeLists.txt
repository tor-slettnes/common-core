## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_shared_messaging_rest)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_messaging_http
)

### Source files in this folder
set(SOURCES
  rest-base.c++
  rest-client.c++
)

### Recepies to build the library
include(BuildLibrary)

