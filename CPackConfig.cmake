## -*- cmake -*-
#===============================================================================
## @file CPackConfig.cmake
## @brief CPack configuration
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

# set(CPACK_PACKAGE_NAME "cc")
# set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/cc")
set(CPACK_PACKAGE_VENDOR "Common Core Authors")
set(CPACK_PACKAGE_CONTACT "Tor Slettnes <tor@slett.net>")

### Debian specific packaging
set(CPACK_DEB_COMPONENT_INSTALL FALSE)
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")

### Python wheels
set(CPACK_PYTHON_DEPENDENCIES "grpcio" "protobuf" "zmq")
set(CPACK_PYTHON_PACKAGE "cc")

