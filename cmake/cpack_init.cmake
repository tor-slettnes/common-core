## -*- cmake -*-
#===============================================================================
## @file CPackInit.cmake
## @brief CPack generic settings.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### General CPack settings
set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_VENDOR}")
set(CPACK_PACKAGE_CONTACT "${PACKAGE_CONTACT}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-${BUILD_NUMBER}")

set(CPACK_GENERATOR "External" "DEB")
set(CPACK_STRIP_FILES ON)
#set(CPACK_COMPONENTS_GROUPING IGNORE)
#set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
#set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_SOURCE_DIR}/CPackConfig.cmake")

#===============================================================================
### DEB generator settings

set(CPACK_DEB_COMPONENT_INSTALL FALSE)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")

if (${CMAKE_CROSSCOMPILING})
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
else()
  execute_process(
    COMMAND dpkg --print-architecture
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
  )
endif()
set(CPACK_SYSTEM_NAME "${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

#===============================================================================
### External generator settings for producing a Python wheel

set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/cpack_wheel.cmake")
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)

#===============================================================================
### Custom settings propagated to external generator

set(CPACK_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}")
set(CPACK_SETTINGS_DIR "${SETTINGS_DIR}")

#===============================================================================
### Include project specific settings
include("${CMAKE_SOURCE_DIR}/CPackConfig.cmake"
  OPTIONAL
  RESULT_VARIABLE CPackConfig_FOUND)

if(CPackConfig_FOUND)
  message(STATUS "Found ${CMAKE_SOURCE_DIR}/CPackConfig.cmake")
else()
  message(STATUS "Did not find ${CMAKE_SOURCE_DIR}/CPackConfig.cmake")
endif()
