## -*- cmake -*-
#===============================================================================
## @file CPackInit.cmake
## @brief CPack generic settings.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Load settings from `defaults.json`
get_build_arg(PACKAGE_NAME "package" "name")
get_build_arg(PACKAGE_VENDOR "package" "vendor")
get_build_arg(PACKAGE_CONTACT "package" "contact")


### General CPack settings
set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${PACKAGE_CONTACT}")
#set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_GENERATOR "External" "DEB")
set(CPACK_STRIP_FILES ON)
#set(CPACK_COMPONENTS_GROUPING IGNORE)
#set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
#set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_SOURCE_DIR}/CPackConfig.cmake")

#===============================================================================
### DEB generator settings

set(CPACK_DEB_COMPONENT_INSTALL TRUE)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_RELEASE "${BUILD_NUMBER}")

#===============================================================================
### External generator settings for producing a Python wheel

set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/cpack_wheel.cmake")
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)

#===============================================================================
### Custom settings propagated to external generator

set(CPACK_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}")
set(CPACK_PYTHON_NAMESPACE "${PYTHON_NAMESPACE}")
set(CPACK_SETTINGS_DIR "${SETTINGS_DIR}")
string(REPLACE "\"" "\\\"" CPACK_PYTHON_DEPENDENCIES "${PYTHON_DEPENDENCIES}")
