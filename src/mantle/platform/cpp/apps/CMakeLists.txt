## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "platform-tools")
endif()

add_subdirectory(platformserver)
add_subdirectory(sysconfig-tool)
add_subdirectory(netconfig-tool)
add_subdirectory(vfs-tool)
add_subdirectory(upgrade-tool)

if(INSTALL_COMPONENT)
  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS platform-settings
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Platform server"
    DESCRIPTION "Platform services: System configuration, networking, filesystem, upgrade, logging"
  )
endif()
