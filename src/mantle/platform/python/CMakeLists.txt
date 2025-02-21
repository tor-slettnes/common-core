## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Package component
if(BUILD_PLATFORM)
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
