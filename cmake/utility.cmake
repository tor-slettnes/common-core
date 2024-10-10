#===============================================================================
## @file build_utilities.cmake
## @brief Misc. utility functions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#===============================================================================
## @fn get_value_or_default
## @brief Get the contents of the specified variable, or else a defaultvalue

function(get_value_or_default OUTPUT_VARIABLE VARIABLE DEFAULT)
  if(${VARIABLE})
    set("${OUTPUT_VARIABLE}" "${${VARIABLE}}" PARENT_SCOPE)
  else()
    set("${OUTPUT_VARIABLE}" "${DEFAULT}" PARENT_SCOPE)
  endif()
endfunction()


#===============================================================================
## @fn get_optional_keyword
## @brief Get the contents of the specified variable, or else a defaultvalue

function(get_optional_keyword KEYWORD VALUE)
  set(_options)
  set(_singleargs OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  get_value_or_default(output_variable
    arg_OUTPUT_VARIABLE
    "${KEYWORD}")

  if(${VALUE})
    set("${output_variable}" "${KEYWORD}" PARENT_SCOPE)
  else()
    set("${output_variable}" "" PARENT_SCOPE)
  endif()
endfunction()


#===============================================================================
## @fn cascade_inherited_property
## @brief
##    Build a list of values stored as a property on the specified target,
##    and subsequently/recursively in its downstream dependents

function(cascade_inherited_property)
  set(_options REMOVE_DUPLICATES)
  set(_singleargs PROPERTY TARGET TARGET_PROPERTY OUTPUT_VARIABLE)
  set(_multiargs DEPENDENCIES INITIAL_VALUE)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(values "${arg_INITIAL_VALUE}")
  foreach(_dep ${arg_DEPENDENCIES})
    get_target_property(inherited_values "${_dep}" "${arg_PROPERTY}")

    if(inherited_values)
      list(APPEND values "${inherited_values}")
    endif()
  endforeach()

  if(arg_REMOVE_DUPLICATES)
    list(REMOVE_DUPLICATES values)
  endif()

  if (arg_TARGET)
    if (arg_TARGET_PROPERTY)
      set(property "${arg_TARGET_PROPERTY}")
    else()
      set(property "${arg_PROPERTY}")
    endif()

    set_target_properties("${arg_TARGET}"
      PROPERTIES "${property}" "${values}")
  endif()

  if (arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}" "${values}" PARENT_SCOPE)
  endif()
endfunction()


#===============================================================================
## @fn get_target_properties_recursively
## @brief
##    Get multiple properties at once for a specified target as well as its
##    dependencies. The result is a list containing the corresponding property
##    values for each target, each joined by SEPARATOR (colon by default).  With
##    the option `ALL_OR_NOTHING`, only targets that have defined all of the
##    requested properties are represented.

function(get_target_properties_recursively)
  set(_options REMOVE_DUPLICATES ALL_OR_NOTHING REQUIRED)
  set(_singleargs SEPARATOR OUTPUT_VARIABLE)
  set(_multiargs PROPERTIES TARGETS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  get_optional_keyword(ALL_OR_NOTHING "${arg_ALL_OR_NOTHING}")
  get_value_or_default(separator arg_SEPARATOR ":")

  set(propslist)
  foreach(target ${arg_TARGETS})
    get_target_properties("${target}"
      PROPERTIES "${arg_PROPERTIES}"
      OUTPUT_VARIABLE values
      ${ALL_OR_NOTHING})

    if(values)
      list(JOIN values "${separator}" props)
      list(APPEND propslist "${props}")
    endif()

    get_target_property(dependencies "${target}" MANUALLY_ADDED_DEPENDENCIES)
    if (dependencies)
      get_target_properties_recursively(
        PROPERTIES "${arg_PROPERTIES}"
        TARGETS ${dependencies}
        SEPARATOR "${separator}"
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
## @fn get_target_properties
## @brief
##    Get multiple target properties at once.  The option `ALL_OR_NOTHING`
##    returns an empty string unless all properties are defined.

function(get_target_properties TARGET)
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
