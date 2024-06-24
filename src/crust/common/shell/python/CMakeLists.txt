## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_common_shell")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  shell.py
)

BuildPython("${TARGET}"
  PACKAGE "${PYTHON_PACKAGE}.common"
  COMPONENT "${PKG_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
)
