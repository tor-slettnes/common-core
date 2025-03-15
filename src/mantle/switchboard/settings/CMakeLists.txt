## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_switchboard_settings")

if(BUILD_SWITCHBOARD)
  set(INSTALL_COMPONENT "switchboard-settings")

  cc_add_settings("${TARGET}"
    SETTINGS_DEPS cc_core_settings
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    DIRECTORIES "."
  )

  cpack_add_component(${INSTALL_COMPONENT}
    GROUP ${INSTALL_GROUP}
    DEPENDS core-settings
    DISPLAY_NAME "Switchboard settings"
    DESCRIPTION "Settings for switchboard application"
  )
endif()
