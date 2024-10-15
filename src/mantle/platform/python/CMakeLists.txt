## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

### Package component
if(INCLUDE_PLATFORM)
  set(INSTALL_COMPONENT "platform-python")
endif()

add_subdirectory(protobuf)
add_subdirectory(platform)

if(INSTALL_COMPONENT)
  cpack_add_component("${INSTALL_COMPONENT}"
    DEPENDS "core-python" "core-settings" "platform-proto" "platform-settings"
    GROUP "${INSTALL_GROUP}"
    DISPLAY_NAME "Platform Services - Python modules"
    DESCRIPTION "Python client moduules for platform services: SysConfig, NetConfig, VFS"
  )
endif()
