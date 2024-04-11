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
)
