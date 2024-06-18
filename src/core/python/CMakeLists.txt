## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
set(TARGET "cc_core_python")

### Package component
set(PKG_COMPONENT "core-python")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Files to be installed as is, without execute permissionss
set(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/generated/buildinfo.py
)

### Subdirectories
set(DIRECTORIES
  core
  protobuf
  messaging
)

### Wheel dependencies
set(PKG_DEPS
  grpcio
  protobuf
  zmq
)

### Populate version/build info
configure_file(buildinfo.py.in "generated/buildinfo.py")

BuildPython("${TARGET}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
  COMPONENT "${PKG_COMPONENT}"
)

### Add a installation package component
cpack_add_component("${PKG_COMPONENT}"
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "Common Core - Python core"
  DESCRIPTION "Python shared components"
  INSTALL_TYPES python
)
