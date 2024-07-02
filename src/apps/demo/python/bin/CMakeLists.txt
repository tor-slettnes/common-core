## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tslettnes@picarro.com>
#===============================================================================

set(TARGET "cc_demo_python_bin")

### Modules to be installed as programs, with execute permissionss
set(SERVER_PROGRAMS
  demo-grpc-server.py
  demo-grpc-async-server.py
  demo-zmq-server.py
)

set(CLIENT_PROGRAMS
  demo-grpc-async-client.py
)

install(
  PROGRAMS ${CLIENT_PROGRAMS}
  TYPE BIN
  COMPONENT "${PKG_COMPONENT}"
)

install(
  PROGRAMS ${SERVER_PROGRAMS}
  TYPE SBIN
  COMPONENT "${PKG_COMPONENT}"
)
