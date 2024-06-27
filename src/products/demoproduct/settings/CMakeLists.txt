## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core Demo
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PKG_COMPONENT "demo-settings")

BuildSettings(COMPONENT ${PKG_COMPONENT})

cpack_add_component(${PKG_COMPONENT}
  GROUP ${PKG_GROUP}
  DISPLAY_NAME "Common Core - DEMO application settings"
  DESCRIPTION "Settings for DEMO application example,"
  "including interactive Python shell and common components."
  "Mainly useful as a template to create wrappers for other products.\n"
  "This package contains product-specific settings files."
)
