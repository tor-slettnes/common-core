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
  PACKAGE_COMPONENT "platform"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)
