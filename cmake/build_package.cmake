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
## @fn cc_cpack_debian_config
## @brief Set a `CPACK_DEBIAN[_GROUPING]_*` value
##    depending on whether component-based packaging is enabled.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_cpack_debian_config VARIABLE_SUFFIX VALUE)
  set(_options APPEND)
  set(_singleargs COMPONENT GROUP)
  set(_multiargs VALUE)
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
  set(_singleargs PREFIX SUFFIX COMPONENT GROUP OUTPUT_VARIABLE)
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
  set(_singleargs PREFIX SUFFIX GLUE COMPONENT GROUP OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(parts ${arg_PREFIX})

  if(CPACK_DEB_COMPONENT_INSTALL)
    if(CPACK_COMPONENTS_GROUPING STREQUAL "IGNORE")
      list(APPEND parts "${arg_COMPONENT}")
    elseif(CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
      list(APPEND parts "${arg_GROUP}")
    endif()
  endif()

  list(APPEND parts ${arg_SUFFIX})
  list(JOIN parts "${arg_GLUE}" variable)
  set("${arg_OUTPUT_VARIABLE}" "${variable}" PARENT_SCOPE)
endfunction()
