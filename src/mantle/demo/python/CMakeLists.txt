## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

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
  COMPONENT demo-python
)

cpack_add_component(demo-python
  DEPENDS core-python demo-protobuf
  GROUP demo
  DISPLAY_NAME "DEMO Example - Python modules"
  DESCRIPTION "Python client and server moduules for demo application example"
)
