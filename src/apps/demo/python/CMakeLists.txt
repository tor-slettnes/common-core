## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Where do these files get installed?
set(PYTHON_PACKAGE demo)

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Modules to be installed as supporting files, without execute permissions
set(FILES
)

### Subdirectories
set(DIRECTORIES
  common
  grpc
  zmq
)

### Common recipe
include(BuildPython)

