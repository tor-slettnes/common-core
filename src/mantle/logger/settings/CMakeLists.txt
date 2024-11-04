## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build settings for Logger service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_logger_settings")

if(INCLUDE_LOGGER)
  set(INSTALL_COMPONENT "logger-settings")

  cc_add_settings("${TARGET}"
    SETTINGS_DEPS cc_core_settings
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    DIRECTORIES "."
  )

  cpack_add_component(${INSTALL_COMPONENT}
    GROUP ${INSTALL_GROUP}
    DEPENDS core-settings
    DISPLAY_NAME "Common Core - Logger service settings"
    DESCRIPTION "Settings for Common Core Logger service."
  )
endif()

