## -*- cmake -*-
#===============================================================================
## @file CPackConfig.cmake
## @brief CPack configuration
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_GENERATOR "DEB")
set(CPACK_STRIP_FILES ON)
set(CPACK_COMPONENTS_GROUPING IGNORE)

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
