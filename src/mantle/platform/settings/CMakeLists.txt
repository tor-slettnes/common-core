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


### Create `upgrade.json` with values from `${CMAKE_SOURCE_DIR}/defaults.json`
get_build_arg(UPGRADE_DOWNLOAD_URL         "upgrade" "download url")
get_build_arg(UPGRADE_PACKAGE_SUFFIX       "upgrade" "package suffix")
get_build_arg(UPGRADE_PACKAGE_SUMMARY_URL  "upgrade" "package summary url")
get_build_arg(UPGRADE_KEYRING              "upgrade" "keyring")

configure_file(upgrade.json.in upgrade.json)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/upgrade.json
  DESTINATION "${SETTINGS_DIR}"
  COMPONENT "${INSTALL_COMPONENT}"
)

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core - common service settings"
  DESCRIPTION "Settings for common/platform applications"
)
