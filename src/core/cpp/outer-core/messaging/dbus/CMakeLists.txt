## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_core_dbus)

set(LIB_DEPS cc_core_glibmm)

set(SOURCES
  dbus-proxycontainer.c++
  dbus-proxywrapper.c++
  dbus-proxywrapper-properties.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
