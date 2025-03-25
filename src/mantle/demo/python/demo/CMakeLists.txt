## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python")

set(PROGRAMS
  shell.py
)

set(FILES
  __init__.py
)

set(DIRECTORIES
  base
  grpc
  native
  zmq
)


cc_add_python("${TARGET}"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  NAMESPACE_COMPONENT "demo"
  PYTHON_DEPS "cc_demo_python_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

if(BUILD_DEMO)
  cc_add_python_wheel("demo-shell"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_demo_settings
    DESCRIPTION "Interactive shell to communicate with DEMO service over gRPC or ZMQ"
    #INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )

  cc_add_python_executable("demo_grpc_server_py"
    PYTHON_DEPS ${TARGET}
    DATA_DEPS cc_demo_settings
    VENV "${PYTHON_VENV}"
    SCRIPT "grpc/server.py"
    RUNTIME_HOOKS "starthook.py"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
    USE_SPEC
  )

  cc_add_python_executable("demo_zmq_server_py"
    PYTHON_DEPS "${TARGET}"
    DATA_DEPS cc_demo_settings
    VENV "${PYTHON_VENV}"
    SCRIPT "zmq/server.py"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
  )
endif()
