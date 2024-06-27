## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "common-shell")

add_subdirectory(bin)
add_subdirectory(python)

cpack_add_component(${PKG_COMPONENT}
  GROUP ${PKG_GROUP}
  DEPENDS core-python
  DISPLAY_NAME "Common Core - Generic product shell"
  DESCRIPTION "Generic template for overall product components, "
  "including interactive Python shell and common components. "
  "Mainly useful as a template to create wrappers for other products.\n"
  "This package contains the interactive Python shell"
)
