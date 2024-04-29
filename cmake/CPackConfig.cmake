## -*- cmake -*-
#===============================================================================
## @file CPackConfig.cmake
## @brief CPack configuration
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

# function(AddPackageComponent COMPONENT)
#   set(_options STRIP)
#   set(_singleargs DISPLAY_NAME DESCRIPTION GROUP)
#   set(_multiargs DEPENDS)
#   cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

#   set(_package_name ${PROJECT_NAME}-${COMPONENT})
#   set(_package_name ${_package_name}_${PROJECT_VERSION})
#   set(_package_name ${_package_name}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
#   set(CPACK_DEBIAN_${COMPONENT}_PACKAGE_NAME ${_package_name} PARENT_SCOPE)
#   set(CPACK_CURRENT_COMPONENT ${COMPONENT} PARENT_SCOPE)

#   foreach(arg ${_singleargs} ${_multiargs})
#     if(arg_${ARG})
#       list(APPEND _args ${arg} "${arg_${arg}}")
#     endif()
#   endforeach()

#   message(STATUS "cpack_add_component(${COMPONENT} ${_args}); package_name=${_package_name}")
#   cpack_add_component("${COMPONENT}" ${_args})
# endfunction()

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
