## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

### Package component
set(PKG_COMPONENT "platform-python")

add_subdirectory(platform)

cpack_add_component("${PKG_COMPONENT}"
  DEPENDS "core-python" "common-settings" "platform-proto" "platform-settings"
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "Platform Services - Python modules"
  DESCRIPTION "Python client moduules for platform services: System, Network, File"
)
