## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build settings for MultiLogger service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cc_add_settings(cc_multilogger_settings
  SETTINGS_DEPS cc_core_settings
  DIRECTORIES "."
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
)
