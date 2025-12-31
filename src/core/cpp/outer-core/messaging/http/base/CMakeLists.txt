## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_http)

### We need cURL for this
find_package(CURL
  REQUIRED COMPONENTS HTTP HTTPS
)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_base
  CURL::libcurl
)

### Source files in this folder
set(SOURCES
  http-utils.c++
  http-client.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)

