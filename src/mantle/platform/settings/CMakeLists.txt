## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_settings")

if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "platform-settings")

  configure_file(upgrade.yaml.in upgrade.yaml)

  cc_add_settings("${TARGET}"
    SETTINGS_DEPS cc_core_settings
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    FILES "${CMAKE_CURRENT_BINARY_DIR}/upgrade.yaml"
    DIRECTORIES "."
  )

  ### Install the settings file
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/upgrade.yaml
    DESTINATION "${SETTINGS_DIR}"
    COMPONENT "${INSTALL_COMPONENT}"
  )

  cpack_add_component(${INSTALL_COMPONENT}
    GROUP ${INSTALL_GROUP}
    DEPENDS core-settings
    DISPLAY_NAME "Common Core - common service settings"
    DESCRIPTION "Settings for common/platform applications"
  )

endif()
