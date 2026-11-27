## -*- cmake -*-

set(TARGET cc_core_udev)

set(LIB_DEPS
  cc_core_common
)

set(PKG_DEPS
  libudev
)

set(SOURCES
  udev-deviceinfo.c++
  udev-enumerator.c++
  udev-monitor.c++
  udev-signals.c++
)

cc_add_library("${TARGET}"
  PKG_DEPS ${PKG_DEPS}
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
