## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python")

### Package component
set(INSTALL_COMPONENT "demo-python")

add_subdirectory(protobuf)
add_subdirectory(demo)

cpack_add_component("${INSTALL_COMPONENT}"
  DEPENDS "core-python" "core-settings" "demo-proto" "demo-settings"
  GROUP "${INSTALL_GROUP}"
  DISPLAY_NAME "DEMO Example - Python modules"
  DESCRIPTION "Python client and server moduules for demo application example"
)
