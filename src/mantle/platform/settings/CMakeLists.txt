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

### Software upgrade settings (normally from `defaults.json`)
get_build_arg(UPGRADE_PACKAGE_SUFFIX
  SETTING "upgrade" "package suffix")

get_build_arg(UPGRADE_SCAN_URL
  SETTING "upgrade" "scan url")

get_build_arg(UPGRADE_VFS_CONTEXT
  SETTING "upgrade" "vfs context")

get_build_arg(UPGRADE_KEYRING
  SETTING "upgrade" "keyring")

configure_file(upgrade.json.in upgrade.json)

### Install the settings file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/upgrade.json
  DESTINATION "${SETTINGS_DIR}"
  COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${INSTALL_GROUP}
  DEPENDS core-settings
  DISPLAY_NAME "Common Core - common service settings"
  DESCRIPTION "Settings for common/platform applications"
)
