## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_COMPONENT platform-tools)

add_subdirectory(platformserver)
add_subdirectory(sysconfig-tool)
add_subdirectory(netconfig-tool)
add_subdirectory(vfs-tool)
add_subdirectory(upgrade-tool)

cpack_add_component(${INSTALL_COMPONENT}
  DEPENDS platform-settings
  GROUP ${INSTALL_GROUP}
  DISPLAY_NAME "Platform server"
  DESCRIPTION "Platform services: System configuration, networking, filesystem, upgrade, logging"
)
