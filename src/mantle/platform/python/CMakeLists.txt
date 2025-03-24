## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python")

set(PROGRAMS
  platform_shell.py
)

set(FILES
)

set(DIRECTORIES
  protobuf
  sysconfig
  netconfig
  vfs
  upgrade
)

set(PYTHON_DEPS
  cc_multilogger_python
  cc_switchboard_python
)

set(PROTO_DEPS
  cc_platform_proto
)

cc_add_python("${TARGET}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
  PYTHON_DEPS ${PYTHON_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_PLATFORM)
  cc_add_python_wheel("platform-clients"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS "cc_platform_settings"
    DESCRIPTION "Python client modules for Platform services"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
