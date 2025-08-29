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
  INSTALL_CONDITION WITH_PYTHON_MODULES
  INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
)

if(BUILD_DEMO)

  #-----------------------------------------------------------------------------
  ## Build Python wheel if `WITH_PYTHON_WHEELS` is set.}

  cc_add_python_wheel("demo"
    PYTHON_DEPS ${TARGET}
    WHEEL_DEPS "core"
    DESCRIPTION "Python DEMO package with gRPC and ZMQ communication examples"
    LOCAL_VENV "venv"
    INSTALL_CONDITION WITH_PYTHON_WHEELS
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    INSTALL_VENV "demo"
    INSTALL_SYMLINKS "bin"
    SCRIPTS demo-client=${PYTHON_NAMESPACE}.demo.grpc.client:main
  )

  #-----------------------------------------------------------------------------
  ## Build Python binaries if `WITH_PYTHON_EXECUTABLES` is set.}

  cc_add_python_executable("demo-grpc-server"
    PROGRAM "demo-grpc-server-py"
    PYTHON_DEPS ${TARGET}
    REQUIREMENTS_DEPS "core-python-requirements"
    SCRIPT "demo/grpc/server.py"
    RUNTIME_HOOKS "demo/starthook.py"
    INSTALL_CONDITION WITH_PYTHON_EXECUTABLES
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
    USE_SPEC
  )

  cc_add_python_executable("demo-zmq-server"
    PROGRAM "demo-zmq-server-py"
    PYTHON_DEPS ${TARGET}
    REQUIREMENTS_DEPS "core-python-requirements"
    SCRIPT "demo/zmq/server.py"
    INSTALL_CONDITION WITH_PYTHON_EXECUTABLES
    INSTALL_COMPONENT "${PYTHON_INSTALL_COMPONENT}"
    TYPE SBIN
  )

endif()
