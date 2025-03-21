## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python_modules")

set(PROGRAMS
  shell.py
)

set(FILES
  __init__.py
)

set(DIRECTORIES
  sysconfig
  netconfig
  vfs
  upgrade
)

set(PYTHON_DEPS
  cc_platform_python_protobuf
  cc_multilogger_python
  cc_switchboard_python
)


cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "platform"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  PYTHON_DEPS ${PYTHON_DEPS}
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(BUILD_PLATFORM)
  cc_add_python_wheel("platform-clients"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS "cc_platform_settings"
    DESCRIPTION "Python client modules for Platform services"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
