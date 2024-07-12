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
  PACKAGE "${CPACK_PYTHON_PACKAGE}.demo"
  COMPONENT "${PKG_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)
