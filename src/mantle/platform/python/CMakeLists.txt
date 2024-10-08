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

add_subdirectory(protobuf)
add_subdirectory(platform)

cpack_add_component("${INSTALL_COMPONENT}"
  DEPENDS "core-python" "core-settings" "platform-proto" "platform-settings"
  GROUP "${INSTALL_GROUP}"
  DISPLAY_NAME "Platform Services - Python modules"
  DESCRIPTION "Python client moduules for platform services: SysConfig, NetConfig, VFS"
)
