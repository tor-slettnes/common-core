## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_multilogger_python")

set(FILES
  __init__.py
)

set(DIRECTORIES
  base
  grpc
)

cc_add_python("${TARGET}"
  NAMESPACE_COMPONENT "multilogger"
  PYTHON_DEPS "cc_multilogger_python_protobuf"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_MULTILOGGER)
  cc_add_python_wheel("multilogger-client"
    PYTHON_DEPS "${TARGET}"
    DATA_DEPS "cc_multilogger_settings"
    DESCRIPTION "Python client modules for MultiLogger service"
    #INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )
endif()
