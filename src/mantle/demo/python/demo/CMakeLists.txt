## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python_modules")

set(PROGRAMS
  shell.py
)

set(FILES
  __init__.py
)

set(DIRECTORIES
  core
  grpc
  native
  zmq
)


cc_add_python("${TARGET}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  NAMESPACE_COMPONENT "demo"
  PYTHON_DEPS "cc_demo_python_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(INCLUDE_DEMO)
  cc_add_python_wheel("demo-shell"
    BUILD_DEPS "python_build"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_demo_settings
    DESCRIPTION "Interactive shell to communicate with DEMO service over gRPC or ZMQ"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  )

  cc_add_python_executable("pydemo_grpc_server"
    BUILD_DEPS "python_pyinstaller"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_demo_settings
    VENV "${PYTHON_VENV}"
    SCRIPT "grpc/server.py"
    RUNTIME_HOOKS "starthook.py"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    TYPE SBIN
  )

  cc_add_python_executable("pydemo_zmq_server"
    PYTHON_DEPS "${TARGET}"
    BUILD_DEPS "python_pyinstaller"
    DATA_DEPS cc_demo_settings
    VENV "${PYTHON_VENV}"
    SCRIPT "zmq/server.py"
    INSTALL_COMPONENT "${INSTALL_COMPONENT}"
    TYPE SBIN
  )
endif()
