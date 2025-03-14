## -*- cmake -*-
#===============================================================================
## @file CPackInit.cmake
## @brief CPack generic settings.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### General CPack settings
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")

### Installation folder
#set(CPACK_PACKAGING_INSTALL_PREFIX "/opt")
cc_get_value_or_default(INSTALL_ROOT
  CPACK_PACKAGING_INSTALL_PREFIX
  "/usr")

#set(CPACK_GENERATOR "External" "DEB")
set(CPACK_STRIP_FILES ON)
set(CPACK_GENERATOR "DEB")

if(PACKAGE_SPLIT_BY_COMPONENT)
  set(CPACK_DEB_COMPONENT_INSTALL TRUE)
  set(CPACK_COMPONENTS_GROUPING "IGNORE")

elseif(PACKAGE_SPLIT_BY_GROUP)
  set(CPACK_DEB_COMPONENT_INSTALL TRUE)
  set(CPACK_COMPONENTS_GROUPING "ONE_PER_GROUP")

else()
  set(CPACK_DEB_COMPONENT_INSTALL FALSE)
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
if(RELEASE_TAG)
  set(CPACK_DEBIAN_PACKAGE_RELEASE "${BUILD_NUMBER}~${RELEASE_TAG}")
else()
  set(CPACK_DEBIAN_PACKAGE_RELEASE "${BUILD_NUMBER}")
endif()
