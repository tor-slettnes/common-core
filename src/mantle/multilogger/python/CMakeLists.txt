## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_multilogger_python")

cc_add_python("${TARGET}"
  DIRECTORIES "protobuf" "multilogger"
  PYTHON_DEPS "cc_core_python"
  PROTO_DEPS "cc_multilogger_proto"
  DATA_DEPS "cc_multilogger_settings"
  INSTALL_CONDITION INSTALL_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_MULTILOGGER)
  cc_add_python_wheel("multilogger_client"
    PYTHON_DEPS "${TARGET}"
    WHEEL_DEPS "core"
    DESCRIPTION "Python client package for MultiLogger service"
    INSTALL_CONDITION INSTALL_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
