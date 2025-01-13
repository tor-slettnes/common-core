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

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "platform"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  PYTHON_DEPS "cc_platform_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(INCLUDE_PLATFORM)
  cc_add_python_wheel("platform-clients"
    BUILD_DEPS "python_build"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_platform_settings
    DESCRIPTION "Python client modules for Platform services"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
