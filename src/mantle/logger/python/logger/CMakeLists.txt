## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_logger_python_modules")

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
  NAMESPACE_COMPONENT "logger"
  PYTHON_DEPS "cc_logger_python_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(INCLUDE_LOGGER)
  cc_add_python_wheel("logger-client"
    BUILD_DEPS "python_build"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_logger_settings
    DESCRIPTION "Python client modules for Logger service"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )
endif()
