## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

configure_file(
  "switchboard.yaml.in"
  "switchboard.yaml"
)

cc_add_settings(cc_switchboard_settings
  SETTINGS_DEPS cc_core_settings
  DIRECTORIES "."
  FILES ${CMAKE_CURRENT_BINARY_DIR}/switchboard.yaml
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
)
