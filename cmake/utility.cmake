#===============================================================================
## @file build_utilities.cmake
## @brief Misc. utility functions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#===============================================================================
## @fn cc_get_value_or_default
## @brief Get the contents of the specified variable, or else a defaultvalue

function(cc_get_value_or_default OUTPUT_VARIABLE VARIABLE DEFAULT)
  if(${VARIABLE})
    set("${OUTPUT_VARIABLE}" "${${VARIABLE}}" PARENT_SCOPE)
  else()
    set("${OUTPUT_VARIABLE}" "${DEFAULT}" PARENT_SCOPE)
  endif()
endfunction()


#===============================================================================
## @fn cc_get_optional_keyword
## @brief Get the contents of the specified variable, or else a defaultvalue

function(cc_get_optional_keyword KEYWORD VALUE)
  set(_options)
  set(_singleargs OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_value_or_default(output_variable
    arg_OUTPUT_VARIABLE
    "${KEYWORD}")

  if(${VALUE})
    set("${output_variable}" "${KEYWORD}" PARENT_SCOPE)
  else()
    set("${output_variable}" "" PARENT_SCOPE)
  endif()
endfunction()

#===============================================================================
## @fn get_quoted_strings
## @brief
##   Tranform a list to a single string of quoted and comma-separated items.

function(cc_join_quoted LIST)
  set(_options)
  set(_singleargs QUOTE LEFTQUOTE RIGHTQUOTE GLUE OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_value_or_default(quote arg_QUOTE "\"")
  cc_get_value_or_default(leftquote arg_LEFTQUOTE "${quote}")
  cc_get_value_or_default(rightquote arg_RIGHTQUOTE "${quote}")
  cc_get_value_or_default(glue arg_GLUE ", ")

  list(TRANSFORM "${LIST}"
    REPLACE "^(.+)$" "${leftquote}\\1${rightquote}"
    OUTPUT_VARIABLE quoted_list)

  list(JOIN quoted_list "${glue}" joined)
  set("${arg_OUTPUT_VARIABLE}" "${joined}" PARENT_SCOPE)
endfunction()



#===============================================================================
## @fn cc_get_target_property_recursively
## @brief
##    Get a property for a specified target as well as its upstream
##    dependencies.  The result is a list containing the corresponding property
##    values for each target, each of which may or may not in turn be a list.

function(cc_get_target_property_recursively)
  set(_options REMOVE_DUPLICATES REQUIRED)
  set(_singleargs OUTPUT_VARIABLE)
  set(_multiargs INITIAL_VALUE PROPERTY TARGETS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(propslist ${arg_INITIAL_VALUE})
  foreach(target ${arg_TARGETS})
    get_target_property(values "${target}" "${arg_PROPERTY}")
    if(values)
      list(APPEND propslist "${values}")
    endif()
    get_target_property(dependencies "${target}" MANUALLY_ADDED_DEPENDENCIES)
    if (dependencies)
      cc_get_target_property_recursively(
        PROPERTY "${arg_PROPERTY}"
        TARGETS "${dependencies}"
        INITIAL_VALUE "${propslist}"
        OUTPUT_VARIABLE propslist)
    endif()
  endforeach()

  if (arg_REMOVE_DUPLICATES)
    list(REMOVE_DUPLICATES propslist)
  endif()

  if (arg_REQUIRED AND NOT propslist)
    message(FATAL_ERROR
      "Target property '${arg_PROPERTY}' is required "
      "but not present present in any of: ${arg_TARGETS}")
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${propslist}" PARENT_SCOPE)
endfunction()



#===============================================================================
## @fn cc_get_target_properties_recursively
## @brief
##    Get multiple properties at once for a specified target as well as its
##    dependencies. The result is a list containing the corresponding property
##    values for each target, each joined by SEPARATOR (colon by default).  With
##    the option `ALL_OR_NOTHING`, only targets that have defined all of the
##    requested properties are represented.

function(cc_get_target_properties_recursively)
  set(_options REMOVE_DUPLICATES ALL_OR_NOTHING REQUIRED)
  set(_singleargs PREFIX SEPARATOR SUFFIX OUTPUT_VARIABLE)
  set(_multiargs INITIAL_VALUE PROPERTIES TARGETS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_optional_keyword(ALL_OR_NOTHING "${arg_ALL_OR_NOTHING}")
  cc_get_value_or_default(separator arg_SEPARATOR ":")

  set(propslist ${arg_INITIAL_VALUE})
  foreach(target ${arg_TARGETS})
    cc_get_target_properties("${target}"
      PROPERTIES "${arg_PROPERTIES}"
      OUTPUT_VARIABLE values
      ${ALL_OR_NOTHING})

    if(values)
      list(JOIN values "${separator}" props)
      list(APPEND propslist "${arg_PREFIX}${props}${arg_SUFFIX}")
    endif()

    get_target_property(dependencies "${target}" MANUALLY_ADDED_DEPENDENCIES)
    if (dependencies)
      cc_get_target_properties_recursively(
        PROPERTIES "${arg_PROPERTIES}"
        TARGETS ${dependencies}
        PREFIX "${arg_PREFIX}"
        SEPARATOR "${separator}"
        SUFFIX "${arg_SUFFIX}"
        OUTPUT_VARIABLE dep_props
        ${ALL_OR_NOTHING})

      list(APPEND propslist ${dep_props})
    endif()
  endforeach()

  if (arg_REMOVE_DUPLICATES)
    list(REMOVE_DUPLICATES propslist)
  endif()

  if (arg_REQUIRED AND NOT propslist)
    message(FATAL_ERROR
      "Target properties '${arg_PROPERTIES}' are required "
      "but not present present in any of: ${arg_TARGETS}")
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${propslist}" PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_get_target_properties
## @brief
##    Get multiple target properties at once.  The option `ALL_OR_NOTHING`
##    returns an empty string unless all properties are defined.

function(cc_get_target_properties TARGET)
  set(_options ALL_OR_NOTHING)
  set(_singleargs OUTPUT_VARIABLE)
  set(_multiargs PROPERTIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(values)
  foreach(prop ${arg_PROPERTIES})
    get_target_property(value "${TARGET}" "${prop}")
    if(value)
      list(APPEND values "${value}")
    elseif(arg_ALL_OR_NOTHING)
      return()
    else()
      list(APPEND values "")
    endif()
  endforeach()

  if(arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}" "${values}" PARENT_SCOPE)
  endif()
endfunction()
