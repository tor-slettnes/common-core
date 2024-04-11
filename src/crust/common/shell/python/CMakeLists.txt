## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  shell.py
)

BuildPython(
  PROGRAMS ${PROGRAMS}
)

