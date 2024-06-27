## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_GROUP "common")

cpack_add_component_group(${PKG_GROUP}
  DISPLAY_NAME "Common Core - common product components"
  DESCRIPTION "Product components that are common to all products,"
  "e.g. global service settings."
)

add_subdirectory(shell)
add_subdirectory(settings)

cpack_add_component(product-common
  GROUP ${PKG_GROUP}
  DEPENDS common-shell common-settings
  DISPLAY_NAME "Common Core - Generic product template"
  DESCRIPTION "Generic template for overall product components, "
  "including interactive Python shell and common components. "
  "Mainly useful as a template to create wrappers for other products."
)

