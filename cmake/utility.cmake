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
## @fn get_target_property_recursively
## @brief
##    Get a property for a specified target as well as its dependencies.
##    The result is returned as a list.

function(get_target_property_recursively OUTPUT_VARIABLE TARGET PROPERTY)

  set(properties)
  get_target_property(value "${TARGET}" "${PROPERTY}")
  if (value)
    set(properties "${value}")
  endif()

  get_target_property(dependencies "${TARGET}" MANUALLY_ADDED_DEPENDENCIES)
  if (dependencies)
    foreach (dep ${dependencies})
      get_target_property_recursively(dep_prop "${dep}" "${PROPERTY}")
      list(APPEND properties "${dep_prop}")
    endforeach()
  endif()

  list(REMOVE_DUPLICATES properties)
  set("${OUTPUT_VARIABLE}" "${properties}" PARENT_SCOPE)
endfunction()

