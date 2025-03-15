## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build switchboard service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_SWITCHBOARD)
  set(INSTALL_COMPONENT "switchboard-utils")
endif()

add_subdirectory(switch-tool)

if(INSTALL_COMPONENT)
  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS switchboard-settings
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Switchboard command line tools"

    DESCRIPTION "Command line tools to communiate with the Switchboard service via gRPC and/or DDS."
  )
endif()
