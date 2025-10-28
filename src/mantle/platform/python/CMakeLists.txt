## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

set(PROGRAMS
  shell.py
)

set(DIRECTORIES
  netconfig
  sysconfig
  upgrade
  vfs
)

cc_add_python("${TARGET}"
  PROGRAMS ${PROGRAMS}
  DIRECTORIES ${DIRECTORIES}
  NAMESPACE_COMPONENT "platform"
  PYTHON_DEPS "cc_multilogger_python" "cc_switchboard_python"
  PROTO_DEPS "cc_platform_proto"
  DATA_DEPS "cc_platform_settings"
  INSTALL_CONDITION INSTALL_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_PLATFORM)
  cc_add_python_wheel("platform_clients"
    PYTHON_DEPS ${TARGET}
    WHEEL_DEPS "core" "multilogger_client" "switchboard_client"
    DESCRIPTION "Python client modules for Platform services"
    INSTALL_CONDITION INSTALL_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
