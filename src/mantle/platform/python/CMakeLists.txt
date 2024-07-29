## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

### Package component
set(INSTALL_COMPONENT "platform-python")

add_subdirectory(platform)
add_subdirectory(protobuf)

cpack_add_component("${INSTALL_COMPONENT}"
  DEPENDS "core-python" "common-settings" "platform-proto" "platform-settings"
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "Platform Services - Python modules"
  DESCRIPTION "Python client moduules for platform services: SysConfig, NetConfig, VFS"
)
