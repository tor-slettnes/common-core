## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Package component
if(BUILD_SWITCHBOARD)
  set(INSTALL_COMPONENT "switchboard-python")
endif()

add_subdirectory(protobuf)
add_subdirectory(switchboard)

if(INSTALL_COMPONENT)
  cpack_add_component("${INSTALL_COMPONENT}"
    DEPENDS "core-python" "core-settings" "switchboard-proto" "switchboard-settings"
    GROUP "${INSTALL_GROUP}"
    DISPLAY_NAME "Switchboard - Python client modules"
    DESCRIPTION "This package contains Python client modules for the Switchboard service."
  )
endif()
