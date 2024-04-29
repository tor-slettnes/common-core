## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Package component
set(PKG_COMPONENT "demo-python")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  demo-grpc-server.py
  demo-grpc-async-server.py
  demo-grpc-async-client.py
  demo-zmq-server.py
)

### Subdirectories
set(DIRECTORIES
  demo
)

### Common recipe
BuildPython(
  PROGRAMS ${PROGRAMS}
  DIRECTORIES ${DIRECTORIES}
  COMPONENT ${PKG_COMPONENT}
)

cpack_add_component(${PKG_COMPONENT}
  DEPENDS core-python demo-proto
  GROUP demo
  DISPLAY_NAME "DEMO Example - Python modules"
  DESCRIPTION "Python client and server moduules for demo application example"
)
