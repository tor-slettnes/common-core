## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "platform-settings")

if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "${TARGET}")

  configure_file(upgrade.json.in upgrade.json)

  BuildSettings("${TARGET}"
    SETTINGS_DEPS "core-settings"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    FILES "${CMAKE_CURRENT_BINARY_DIR}/upgrade.json"
    DIRECTORIES "./"
  )

  ### Install the settings file
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/upgrade.json
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
