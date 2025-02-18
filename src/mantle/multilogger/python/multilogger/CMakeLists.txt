## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_multilogger_python_modules")

set(PROGRAMS
)

set(FILES
)

set(DIRECTORIES
  base
  grpc
)

cc_add_python("${TARGET}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  NAMESPACE_COMPONENT "multilogger"
  PYTHON_DEPS "cc_multilogger_python_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(INCLUDE_LOGGER)
  cc_add_python_wheel("multilogger-client"
    BUILD_DEPS "python_build"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_multilogger_settings
    DESCRIPTION "Python client modules for MultiLogger service"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
