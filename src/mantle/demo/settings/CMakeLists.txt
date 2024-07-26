## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT "demo-settings")

BuildSettings(INSTALL_COMPONENT "${INSTALL_COMPONENT}")

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core - DEMO service settings"
  DESCRIPTION "Settings for Common Core DEMO service."
)
