## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core Demo
## @author Tor Slettnes
#===============================================================================

cc_add_settings(cc_demo_settings
  SETTINGS_DEPS cc_core_settings
  DIRECTORIES "."
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
)
