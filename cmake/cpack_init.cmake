## -*- cmake -*-
#===============================================================================
## @file CPackInit.cmake
## @brief CPack generic settings.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Debian package settings
get_build_arg(PACKAGE_NAME_PREFIX
  SETTING "package" "name prefix")

get_build_arg(PACKAGE_VENDOR
  SETTING "package" "vendor")

get_build_arg(PACKAGE_CONTACT
  SETTING "package" "contact")

get_build_arg(PACKAGE_SPLIT_BY_COMPONENT
  SETTING "package" "split by component")

get_build_arg(PACKAGE_SPLIT_BY_GROUP
  SETTING "package" "split by group")


get_build_arg(PYTHON_DEPENDENCIES
  SETTING "python" "dependencies")


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


