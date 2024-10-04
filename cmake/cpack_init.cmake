## -*- cmake -*-
#===============================================================================
## @file CPackInit.cmake
## @brief CPack generic settings.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### General CPack settings
set(CPACK_PACKAGE_NAME "${PACKAGE_NAME_PREFIX}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${PACKAGE_CONTACT}")
#set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")

#set(CPACK_GENERATOR "External" "DEB")
set(CPACK_GENERATOR "DEB")
set(CPACK_STRIP_FILES ON)

if(PACKAGE_SPLIT_BY_COMPONENT)
  set(CPACK_COMPONENTS_GROUPING "IGNORE")
  set(CPACK_DEB_COMPONENT_INSTALL TRUE)
elseif (PACKAGE_SPLIT_BY_GROUP)
  set(CPACK_COMPONENTS_GROUPING "ONE_PER_GROUP")
  set(CPACK_DEB_COMPONENT_INSTALL TRUE)
else()
  set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
endif()

#===============================================================================
### DEB generator settings

### Generate automatic package dependencies based on shared lib references
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

### Enable inter-component dependencies
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

### Use Debian policy compliant names for generated `.deb` files
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

### Use BUILD_NUMBER as Debian release number
set(CPACK_DEBIAN_PACKAGE_RELEASE "${BUILD_NUMBER}")

#===============================================================================
### External generator settings for producing a Python wheel

### CMake script to generate Python wheel
set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/cpack_wheel.cmake")

### Create a staging folder for external generation
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)

#===============================================================================
### Custom settings propagated to PyInstall
set(CPACK_PYTHON_VENV "${CMAKE_SOURCE_DIR}/venv")

#===============================================================================
### Custom settings propagated to external generator

set(CPACK_BUILD_NUMBER "${BUILD_NUMBER}")
set(CPACK_SETTINGS_DIR "${SETTINGS_DIR}")
set(CPACK_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}")
set(CPACK_PYTHON_NAMESPACE "${PYTHON_NAMESPACE}")
string(REPLACE "\"" "\\\"" CPACK_PYTHON_DEPENDENCIES "${PYTHON_DEPENDENCIES}")


#===============================================================================

#set(CPACK_INSTALL_SCRIPTS "${CMAKE_CURRENT_LIST_DIR}/cpack_install.cmake")
#set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_CURRENT_LIST_DIR}/cpack_pyinstall.cmake")

