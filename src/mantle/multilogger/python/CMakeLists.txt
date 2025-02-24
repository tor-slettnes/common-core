## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Package component
if(BUILD_LOGGER)
  set(INSTALL_COMPONENT "multilogger-python")
endif()

add_subdirectory(protobuf)
add_subdirectory(multilogger)

if(INSTALL_COMPONENT)
  cpack_add_component("${INSTALL_COMPONENT}"
    DEPENDS "core-python" "core-settings" "multilogger-proto" "multilogger-settings"
    GROUP "${INSTALL_GROUP}"
    DISPLAY_NAME "DEMO Example - Python modules"
    DESCRIPTION "Python client and server moduules for logging service"
  )
endif()
