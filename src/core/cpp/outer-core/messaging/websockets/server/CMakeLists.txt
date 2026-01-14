## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build shared WebSockets wrappers
## @author Tor Slettnes
#===============================================================================

### Name of this library.
set(TARGET cc_core_messaging_ws_server)

set(PKG_DEPS
  asio
)

### Library dependencies.
set(LIB_DEPS
  cc_core_messaging_ws_base
)

### Source files in this folder
set(SOURCES
  ws-server.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  PKG_DEPS ${PKG_DEPS}
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
