## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "demo-settings")

BuildSettings(COMPONENT ${PKG_COMPONENT})

cpack_add_component(${PKG_COMPONENT}
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core - DEMO service settings"
  DESCRIPTION "Settings for Common Core DEMO service."
)
