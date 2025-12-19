## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cc_add_settings(cc_switchboard_settings
  SETTINGS_DEPS cc_core_settings
  DIRECTORIES "." RECURSE
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
)
