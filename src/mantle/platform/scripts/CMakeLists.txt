## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "platform-scripts")

add_subdirectory(bin)

cpack_add_component(${PKG_COMPONENT}
  DEPENDS "platform-python"
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core Platform Services - launcher scripts"
  DESCRIPTION "Common Core Platform Services launcher scripts"
)

