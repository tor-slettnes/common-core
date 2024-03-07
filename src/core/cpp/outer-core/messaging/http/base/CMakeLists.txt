## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_http)

### We need cURL for this
find_package(CURL
  REQUIRED COMPONENTS HTTP HTTPS)

### Library type
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_common
  CURL::libcurl
)

### Source files in this folder
set(SOURCES
  http-utils.c++
  http-client.c++
)

### Recepies to build the library
include(BuildLibrary)

