## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python")

cc_add_python("${TARGET}"
  DIRECTORIES "protobuf" "demo"
  PYTHON_DEPS "cc_core_python"
  PROTO_DEPS "cc_demo_proto"
  DATA_DEPS "cc_demo_settings"
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_DEMO)
  cc_add_python_wheel("demo"
    PYTHON_DEPS ${TARGET}
    DESCRIPTION "Python DEMO package with gRPC and ZMQ communication examples"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
  )

  cc_add_python_executable("demo_grpc_server_py"
    PYTHON_DEPS ${TARGET}
    VENV "${PYTHON_VENV}"
    SCRIPT "demo/grpc/server.py"
    RUNTIME_HOOKS "demo/starthook.py"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
    USE_SPEC
  )

  cc_add_python_executable("demo_zmq_server_py"
    PYTHON_DEPS "${TARGET}"
    VENV "${PYTHON_VENV}"
    SCRIPT "demo/zmq/server.py"
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
  )

endif()
