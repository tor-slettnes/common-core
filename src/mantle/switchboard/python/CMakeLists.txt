## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_switchboard_python")

cc_add_python("${TARGET}"
  DIRECTORIES "protobuf" "switchboard"
  PYTHON_DEPS "cc_core_python"
  PROTO_DEPS "cc_switchboard_proto"
  DATA_DEPS "cc_switchboard_settings"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_SWITCHBOARD)
  cc_add_python_wheel("switchboard_client"
    PYTHON_DEPS "${TARGET}"
    DESCRIPTION "Python client package for Switchboard service"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
