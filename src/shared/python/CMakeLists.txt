## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
add_library(cc_shared_python INTERFACE)

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Modules to be installed as supporting files, without execute permissions
set(FILES
  jsonreader.py
  paths.py
  safe_invoke.py
  scalar_types.py
  settingsstore.py
  stringexpander.py
)

### Subdirectories
set(DIRECTORIES
  ipc
)

configure_file(buildinfo.py.in buildinfo.py)
list(APPEND FILES ${CMAKE_CURRENT_BINARY_DIR}/buildinfo.py)

include(BuildPython)

