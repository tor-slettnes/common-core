## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_switchboard_python")

set(DIRECTORIES
  base
  grpc
)

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "switchboard"
  PYTHON_DEPS "cc_switchboard_python_protobuf"
  DIRECTORIES "${DIRECTORIES}"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_SWITCHBOARD)
  cc_add_python_wheel("switchboard-client"
    PYTHON_DEPS "${TARGET}"
    DATA_DEPS "cc_switchboard_settings"
    DESCRIPTION "Python client modules for Switchboard service"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
