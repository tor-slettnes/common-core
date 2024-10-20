## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_core_glibmm)

set(LIB_DEPS cc_core_common)

set(PKG_DEPS
  glibmm-2.4
  giomm-2.4
)

set(SOURCES
  glib-errors.c++
  glib-variant.c++
  glib-mainloop.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PKG_DEPS ${PKG_DEPS}
  SOURCES ${SOURCES}
)

