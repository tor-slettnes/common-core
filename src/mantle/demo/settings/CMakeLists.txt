## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "demo-settings")
set(INSTALL_COMPONENT "${TARGET}")

BuildSettings("${TARGET}"
  SETTINGS_DEPS "core-settings"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${INSTALL_GROUP}
  DEPENDS core-settings
  DISPLAY_NAME "Common Core - DEMO service settings"
  DESCRIPTION "Settings for Common Core DEMO service."
)
