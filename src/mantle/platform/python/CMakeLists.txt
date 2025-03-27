## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

cc_add_python("${TARGET}"
  PROGRAMS "platform_shell.py"
  DIRECTORIES "protobuf" "sysconfig" "netconfig" "vfs" "upgrade"
  PYTHON_DEPS "cc_multilogger_python" "cc_switchboard_python"
  PROTO_DEPS "cc_platform_proto"
  DATA_DEPS "cc_platform_settings"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_PLATFORM)
  cc_add_python_wheel("platform_clients"
    PYTHON_DEPS ${TARGET}
    DESCRIPTION "Python client modules for Platform services"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
