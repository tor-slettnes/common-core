## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python")

### Package component
set(PKG_COMPONENT "demo-python")

add_subdirectory(protobuf)
add_subdirectory(demo)
add_subdirectory(bin)

cpack_add_component("${PKG_COMPONENT}"
  DEPENDS "core-python" "common-settings" "demo-proto" "demo-settings"
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "DEMO Example - Python modules"
  DESCRIPTION "Python client and server moduules for demo application example"
)
