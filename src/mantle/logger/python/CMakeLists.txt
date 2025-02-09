## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_logger_python")

### Package component
if(INCLUDE_LOGGER)
  set(INSTALL_COMPONENT "logger-python")
endif()

add_subdirectory(protobuf)
add_subdirectory(logger)

if(INSTALL_COMPONENT)
  cpack_add_component("${INSTALL_COMPONENT}"
    DEPENDS "core-python" "core-settings" "logger-proto" "logger-settings"
    GROUP "${INSTALL_GROUP}"
    DISPLAY_NAME "DEMO Example - Python modules"
    DESCRIPTION "Python client and server moduules for logging service"
  )
endif()
