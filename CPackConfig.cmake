## -*- cmake -*-
#===============================================================================
## @file CPackConfig.cmake
## @brief CPack configuration
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(CPACK_PACKAGE_NAME "cc")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/cc")
set(CPACK_PACKAGE_VENDOR "Tor Slettnes")
set(CPACK_DEB_COMPONENT_INSTALL TRUE)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tor Slettnes <tor@slett.net>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
