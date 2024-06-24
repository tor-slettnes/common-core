## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_demo_python")

### Package component
set(PKG_COMPONENT "demo-python")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  demo-grpc-server.py
  demo-grpc-async-server.py
  demo-grpc-async-client.py
  demo-zmq-server.py
)

set(DIRECTORIES
  demo
  protobuf
)

### Common recipe
BuildPython("${TARGET}"
  PACKAGE "${PYTHON_PACKAGE}"
  COMPONENT "${PKG_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
  DIRECTORIES "${DIRECTORIES}"
)

cpack_add_component("${PKG_COMPONENT}"
  DEPENDS core-python demo-proto
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "DEMO Example - Python modules"
  DESCRIPTION "Python client and server moduules for demo application example"
)
