## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "platform-scripts")

  add_subdirectory(bin)

  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS "platform-python"
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Common Core Platform Services - launcher scripts"
    DESCRIPTION "Common Core Platform Services launcher scripts"
  )
endif()


