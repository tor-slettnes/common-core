## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(BUILD_DEMO)
  set(INSTALL_COMPONENT "demo-scripts")

  add_subdirectory(bin)

  cpack_add_component(${INSTALL_COMPONENT}
    DEPENDS "demo-python"
    GROUP ${INSTALL_GROUP}
    DISPLAY_NAME "Common Core DEMO - launcher scripts"
    DESCRIPTION "Shell script to launch interactive Python shell for the Common Core DEMO service"
  )
endif()

