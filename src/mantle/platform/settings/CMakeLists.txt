## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

configure_file(upgrade.yaml.in upgrade.yaml)

cc_add_settings(cc_platform_settings
  SETTINGS_DEPS cc_core_settings
  FILES "${CMAKE_CURRENT_BINARY_DIR}/upgrade.yaml"
  DIRECTORIES "."
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
)
