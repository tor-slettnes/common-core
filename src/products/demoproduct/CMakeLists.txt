## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_GROUP "demo")

cpack_add_component_group(${PKG_GROUP}
  DISPLAY_NAME "Common Core - DEMO example"
  DESCRIPTION "Product components for DEMO example,"
  "including service settings and interactive shell."
)

add_subdirectory(shell)
add_subdirectory(settings)

cpack_add_component(product-demo
  GROUP ${PKG_GROUP}
  DEPENDS demo-shell demo-settings demo-server demo-utils
  DISPLAY_NAME "Common Core - Generic product template"
  DESCRIPTION "Prouct components for DEMO example.\n"
  "This is an empty package to provide the needed dependencies."
)

