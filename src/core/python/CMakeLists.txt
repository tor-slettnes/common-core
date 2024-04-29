## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
add_library(cc_core_python INTERFACE)

### Package component
set(PKG_COMPONENT "core-python")

### Python module
set(MODULE "cc")

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Files to be installed as is, without execute permissionss
configure_file(buildinfo.py.in buildinfo.py)

set(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/buildinfo.py
)


### Subdirectories
set(DIRECTORIES
  core
  messaging
  io
)

include(BuildPython)
BuildPython(
  MODULE "${MODULE}"
  PROGRAMS ${PROGRAMS}
  FILES ${FILES}
  DIRECTORIES ${DIRECTORIES}
  COMPONENT ${PKG_COMPONENT}
)

cpack_add_component(${PKG_COMPONENT}
  GROUP ${PKG_GROUP}
  DEPENDS core-protobuf
  DISPLAY_NAME "Common ProtoBuf/gRPC modules"
  DESCRIPTION "Generated Protocol Buffers/gRPC modules for common components"
)
