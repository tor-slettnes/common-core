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

BuildPython("${TARGET}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  NAMESPACE_COMPONENT "demo"
  PYTHON_DEPS "cc_demo_python_protobuf"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)


BuildPythonExecutable("pydemo_grpc_server"
  PYTHON_DEPS "${TARGET}"
  BUILD_DEPS "python_pyinstaller"
  VENV "${PYTHON_VENV}"
  SCRIPT "grpc/server.py"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  TYPE SBIN
)

BuildPythonExecutable("pydemo_zmq_server"
  PYTHON_DEPS "${TARGET}"
  BUILD_DEPS "python_pyinstaller"
  VENV "${PYTHON_VENV}"
  SCRIPT "zmq/server.py"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  TYPE SBIN
  DIRECTORY_BUNDLE
)
