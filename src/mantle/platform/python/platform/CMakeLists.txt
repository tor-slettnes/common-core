## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_platform_python_modules")

set(PROGRAMS
  shell.py
)

set(FILES
  __init__.py
)

set(DIRECTORIES
  system
  network
)

BuildPython("${TARGET}"
  PACKAGE "${CPACK_PYTHON_PACKAGE}.platform"
  COMPONENT "${PKG_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)
