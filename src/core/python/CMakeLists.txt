## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
add_library(cc_core_python INTERFACE)

### Python module
set(MODULE "cc")

# ### Modules to be installed as programs, with execute permissionss
# set(PROGRAMS
# )

### Files to be installed as is, without execute permissionss
set(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/buildinfo.py
)

### Subdirectories
set(DIRECTORIES
  core
  messaging
  io
)

configure_file(buildinfo.py.in buildinfo.py)
include(BuildPython)

