## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build shared WebSockets wrappers
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_ws_base)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_base
  cc_core_common
)

### Source files in this folder
set(SOURCES
  ws-endpoint.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_WEBSOCKETS=1)

## Experimental interface ahead.
# cc_add_external_dependency(${TARGET} libzmq INCLUDE_DIRS LIBRARIES)
# cc_add_external_dependency(${TARGET} cppzmq INCLUDE_DIRS)
