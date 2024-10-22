## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_demo_settings")

if(INCLUDE_DEMO)
  set(INSTALL_COMPONENT "demo-settings")

  cc_add_settings("${TARGET}"
    SETTINGS_DEPS cc_core_settings
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    DIRECTORIES "."
  )

  cpack_add_component(${INSTALL_COMPONENT}
    GROUP ${INSTALL_GROUP}
    DEPENDS core-settings
    DISPLAY_NAME "Common Core - DEMO service settings"
    DESCRIPTION "Settings for Common Core DEMO service."
  )
endif()

