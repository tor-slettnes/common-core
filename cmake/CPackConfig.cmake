## -*- cmake -*-
#===============================================================================
## @file CPackConfig.cmake
## @brief CPack configuration
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### General CPack settings
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_GENERATOR External DEB)
set(CPACK_STRIP_FILES ON)
#set(CPACK_COMPONENTS_GROUPING IGNORE)
#set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)

#===============================================================================
### DEB generator settings

set(CPACK_DEB_COMPONENT_INSTALL TRUE)
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
set(CPACK_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}")
set(CPACK_SETTINGS_DIR "${SETTINGS_DIR}")
