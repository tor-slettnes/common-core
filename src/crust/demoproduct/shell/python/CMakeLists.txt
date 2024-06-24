## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_shell")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  shell.py
)

BuildPython("${TARGET}"
  PACKAGE "${PYTHON_PACKAGE}.demo"
  PROGRAMS "${PROGRAMS}"
  COMPONENT "${PKG_COMPONENT}"
)

# BuildPythonWheel(common_core_wheel
#   PACKAGE "common_core"
#   COMPONENT "python-core"
#   CONTENTS "${PROGRAMS}"
#   PYTHON_DEPS cc_core_python cc_demo_python
#   PROTO_DEPS cc_core_proto cc_demo_proto
#   PKG_DEPS grpcio protobuf zmq
# )

