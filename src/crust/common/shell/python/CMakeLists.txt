## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a custom target for the Python Wheel
set(TARGET "cc_shell_common")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  shell.py
)

BuildPython("${TARGET}"
  PROGRAMS "${PROGRAMS}"
  COMPONENT "${PKG_COMPONENT}"
)
