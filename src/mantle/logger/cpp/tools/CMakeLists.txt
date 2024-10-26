## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT "logger-tools")

add_subdirectory(logger-tool)

if(INSTALL_COMPONENT)
  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS logger-settings
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Platform server"
    DESCRIPTION "Platform services: System configuration, networking, filesystem, upgrade, logger"
  )
endif()
