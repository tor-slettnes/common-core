## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "core-settings")
set(INSTALL_COMPONENT "${TARGET}")

BuildSettings("${TARGET}"
  INSTALL_COMPONENT "${INSTALL_COMPONENT}")

cpack_add_component(${INSTALL_COMPONENT}
  GROUP ${INSTALL_GROUP}
  DISPLAY_NAME "Common Core - Common product settings"
  DESCRIPTION "Generic template for overall product components, "
  "including interactive Python shell and common components. "
  "Mainly useful as a template to create wrappers for other products.\n"
  "This package contains product-specific settings files."
)
