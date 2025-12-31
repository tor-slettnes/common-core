## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_core_dbus)

set(LIB_DEPS cc_core_glibmm)

set(SOURCES
  dbus-proxycontainer.c++
  dbus-proxywrapper.c++
  dbus-proxywrapper-properties.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
