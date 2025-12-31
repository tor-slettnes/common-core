## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_platform_netconfig_dbus)

set(LIB_DEPS
  cc_core_dbus
  cc_platform_netconfig_base
  cc_platform_sysconfig_base
)

set(SOURCES
  netconfig-providers-dbus.c++
  netconfig-dbus.c++
  nm-wrappers.c++
  nm-structs.c++
  nm-settings.c++
  nm-settings-connection.c++
  nm-manager.c++
  nm-accesspoint.c++
  nm-connection-active.c++
  nm-device.c++
  nm-device-wired.c++
  nm-device-wireless.c++
  nm-ipconfig.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
