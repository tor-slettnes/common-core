## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes
#===============================================================================

set(TARGET "cc_core_settings")

cc_add_settings("cc_core_settings"
  INSTALL_COMPONENT "${SETTINGS_INSTALL_COMPONENT}"
  DIRECTORIES "."
)
