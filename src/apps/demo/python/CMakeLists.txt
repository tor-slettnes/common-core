## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules for Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
  demo_client.py
)

### Modules to be installed as supporting files, without execute permissions
set(FILES
)

### Subdirectories
set(DIRECTORIES
)

### Common recipe
include(BuildPython)

