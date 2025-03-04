## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


#===============================================================================
## @fn cc_cpack_config
## @brief Append a setting to `cc_cpack_config.cmake`, to be consumed by `cpack`.

function(cc_cpack_config VARIABLE VALUE)
  set(_options APPEND)
  set(_singleargs)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(REPLACE "\"" "\\\"" _value "${VALUE}")
  string(TOUPPER "${VARIABLE}" _variable)

  if(arg_APPEND)
    set(_command "list(APPEND ${_variable} \"${_value}\")")
  else()
    set(_command "set(${_variable} \"${_value}\")")
  endif()

  file(APPEND "${CMAKE_BINARY_DIR}/CPackConfig.cmake" "${_command}\n")
endfunction()

#===============================================================================
## @fn cc_cpack_add_group
## @brief
##   Wrapper for `cmake_add_component_group()` with added dependency options.

function(cc_cpack_add_group GROUP)
  set(_options)
  set(_singleargs
    DISPLAY_NAME               # One-line summary to describe this package group
    DESCRIPTION                # Detailed description of this package group
  )
  set(_multiargs
    GROUP_DEPS                 # Other cpack component groups on which we depend
    PACKAGE_DEPS               # Other (full) package names on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cpack_add_component_group("${GROUP}"
    DISPLAY_NAME "${DISPLAY_NAME}"
    DESCRIPTION "${DESCRIPTION}")

  cc_cpack_set_group_dependencies("${GROUP}" DEPENDS
    GROUPS ${arg_GROUP_DEPS}
    PACKAGES ${arg_PACKAGE_DEPS}
  )

endfunction()

#===============================================================================
## @fn cc_cpack_add_group_dependencies

function(cc_cpack_set_group_dependencies TARGET RELATIONSHIP)
  set(_options)
  set(_singleargs)
  set(_multiargs
    GROUPS     # CPack component groups on which TARGET will have a relationship
    PACKAGES   # Full package names on which TARGET will have a relationship
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(TOUPPER "${TARGET}" upcase_target)
  cc_get_value_or_default(relationship RELATIONSHIP DEPENDS)

  list(TRANSFORM arg_GROUPS
    PREPEND "${PACKAGE_NAME_PREFIX}-"
    OUTPUT_VARIABLE ancestors
  )

  list(APPEND ancestors ${arg_PACKAGES})
  list(JOIN ancestors ", " value)

  cc_cpack_config(
    "CPACK_DEBIAN_${upcase_target}_PACKAGE_${relationship}"
    "${value}"
  )
endfunction()


#===============================================================================
## @fn cc_cpack_debian_config
## @brief Set a `CPACK_DEBIAN[_GROUPING]_*` value
##    depending on whether component-based packaging is enabled.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_cpack_debian_config VARIABLE_SUFFIX VALUE)
  set(_options APPEND)
  set(_singleargs
    COMPONENT # CPack component name, for when creating one package per component
    GROUP     # CPack component group, for when creating one package per group
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_cpack_debian_variable("${VARIABLE_SUFFIX}"
    COMPONENT "${arg_COMPONENT}"
    GROUP "${arg_GROUP}"
    OUTPUT_VARIABLE variable_name)

  cc_get_optional_keyword(APPEND "${arg_APPEND}")
  cc_cpack_config("${variable_name}" "${VALUE}" ${APPEND})
endfunction()


#===============================================================================
## @fn cc_get_cpack_debian_variable
## @brief Obtain variable name corresponding to the provided suffix,
##    with with COMPONENT or GROUP inserted if the corresponding component-based
##    packaging is used.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_get_cpack_debian_variable VARIABLE_SUFFIX)
  set(_options)
  set(_singleargs
    PREFIX         # CPackConfig variable prefix (normally `CPACK_DEBIAN`)
    SUFFIX         # CPackConfig variable suffix
    COMPONENT      # CPack component for when creating one package per component
    GROUP          # CPack group for when creating one package per group
    OUTPUT_VARIABLE # Variable in which to assign the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_cpack_debian_grouping(
    PREFIX "CPACK_DEBIAN"
    SUFFIX "${VARIABLE_SUFFIX}"
    GLUE "_"
    COMPONENT "${arg_COMPONENT}"
    GROUP "${arg_GROUP}"
    OUTPUT_VARIABLE variable)

  string(TOUPPER "${variable}" upcase_variable)
  set("${arg_OUTPUT_VARIABLE}" "${upcase_variable}" PARENT_SCOPE)
endfunction()


#===============================================================================
## @fn cc_get_cpack_debian_grouping
## @brief
##    Obtain a string containing COMPONENT or GROUP if and only if
##    the corresponding component-based packaging is used.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_get_cpack_debian_grouping)
  set(_options)
  set(_singleargs
    PREFIX         # CPackConfig variable prefix
    SUFFIX         # CPackConfig variable suffix
    GLUE           # Delimiter beween prefix/selection/suffix
    COMPONENT      # CPack component for when creating one package per component
    GROUP          # CPack group for when creating one package per group
    OUTPUT_VARIABLE # Variable in which to store the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(parts ${arg_PREFIX})

  if(CPACK_DEB_COMPONENT_INSTALL)
    if(arg_COMPONENT AND CPACK_COMPONENTS_GROUPING STREQUAL "IGNORE")
      list(APPEND parts ${arg_COMPONENT})
    elseif(arg_GROUP AND CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
      list(APPEND parts ${arg_GROUP})
    endif()
  endif()

  list(APPEND parts ${arg_SUFFIX})
  list(JOIN parts "${arg_GLUE}" variable)
  set("${arg_OUTPUT_VARIABLE}" "${variable}" PARENT_SCOPE)
endfunction()
