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
  ${CMAKE_CURRENT_BINARY_DIR}/generated/__init__.py
  ${CMAKE_CURRENT_BINARY_DIR}/generated/buildinfo.py
)

### Subdirectories
set(DIRECTORIES
  core
  protobuf
  messaging
  # generated
)

### Wheel dependencies
set(PKG_DEPS
  grpcio
  protobuf
  zmq
)

### Populate version/build info
configure_file(__init__.py.in "generated/__init__.py")
configure_file(buildinfo.py.in "generated/buildinfo.py")

BuildPython("${TARGET}"
  PACKAGE "${CPACK_PYTHON_PACKAGE}"
  COMPONENT "${PKG_COMPONENT}"
  PROGRAMS "${PROGRAMS}"
  FILES "${FILES}"
  DIRECTORIES "${DIRECTORIES}"
)

# BuildPythonWheel("${TARGET}_wheel"
#   CONTENTS ${DIRECTORIES} ${PROGRAMS} ${FILES}
#   COMPONENT "${PKG_COMPONENT}"
#   PROTO_DEPS "cc_core_proto"
# )


### Add a installation package component
cpack_add_component("${PKG_COMPONENT}"
  GROUP "${PKG_GROUP}"
  DISPLAY_NAME "Common Core - Python core"
  DESCRIPTION "Python shared components"
  INSTALL_TYPES python
)
