## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "demo-shell")

add_subdirectory(bin)
add_subdirectory(python)

cpack_add_component(${PKG_COMPONENT}
  GROUP ${PKG_GROUP}
  DEPENDS core-python
  DISPLAY_NAME "DEMO example - Interactive shell"
  DESCRIPTION "Interactive shell template for DEMO example."
)

