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
  PACKAGE_COMPONENT "demo"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)
