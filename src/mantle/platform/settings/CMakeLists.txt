## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT "platform-settings")

BuildSettings(
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
)

### Generate `upgrade.json` with values from `build_args.cmake`
configure_file(upgrade.json.in upgrade.json)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/upgrade.json
  DESTINATION "${SETTINGS_DIR}"
  COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${PKG_GROUP}
  DEPENDS core-settings
  DISPLAY_NAME "Common Core - common service settings"
  DESCRIPTION "Settings for common/platform applications"
)
