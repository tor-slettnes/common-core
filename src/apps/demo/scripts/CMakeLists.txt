## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "demo-scripts")

add_subdirectory(bin)

cpack_add_component(${PKG_COMPONENT}
  DEPENDS "demo-python"
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core DEMO - launcher scripts"
  DESCRIPTION "Shell script to launch interactive Python shell for the Common Core DEMO service"
)
