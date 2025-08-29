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
  INSTALL_CONDITION WITH_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_SWITCHBOARD)
  cc_add_python_wheel("switchboard_client"
    PYTHON_DEPS "${TARGET}"
    WHEEL_DEPS "core"
    DESCRIPTION "Python client package for Switchboard service"
    INSTALL_CONDITION WITH_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
