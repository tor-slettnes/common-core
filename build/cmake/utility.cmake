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
## @fn cc_get_ternary
## @brief Set `OUTPUT_VARIABLE` to ${ON_EXPR} or ${OFF_EXPR} depending on ${CONDITION_VARIABLE}

function(cc_get_ternary OUTPUT_VARIABLE CONDITION_VARIABLE ON_EXPR OFF_EXPR)
  if("${CONDITION_VARIABLE}")
    set("${OUTPUT_VARIABLE}" "${ON_EXPR}" PARENT_SCOPE)
  else()
    set("${OUTPUT_VARIABLE}" "${OFF_EXPR}" PARENT_SCOPE)
  endif()
endfunction()

#===============================================================================
## @fn cc_get_optional_keyword
## @brief Set output variable KEYWORD to "KEYWORD" if PRESENT evaluates to true

function(cc_get_optional_keyword KEYWORD PRESENT)
  if(PRESENT)
    set("${KEYWORD}" "${KEYWORD}" PARENT_SCOPE)
  else()
    set("${KEYWORD}" "" PARENT_SCOPE)
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
        PROPERTIES ${arg_PROPERTIES}
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



#===============================================================================
## @fn cc_get_staging_list
## @brief Get a recursive list of source + staged files

function(cc_get_staging_list)
  set(_options CONFIGURE_DEPENDS)
  set(_singleargs OUTPUT_DIR SOURCES_VARIABLE OUTPUTS_VARIABLE)
  set(_multiargs FILES DIRECTORIES FILENAME_PATTERN)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(sources)
  set(outputs)

  foreach(path ${arg_FILES})
    cmake_path(ABSOLUTE_PATH path NORMALIZE
      OUTPUT_VARIABLE abs_source)

    cmake_path(GET path
      FILENAME filename)

    list(APPEND sources "${abs_source}")
    list(APPEND outputs "${arg_OUTPUT_DIR}/${filename}")
  endforeach()

  ### We process each directory separately in case they are not located
  ### directly below ${CMAKE_CURRENT_SOURCE_DIR}. Paths are then constructed
  ### as relative to the parent folder of each directory (${anchor_dir}, which
  ### may or may not be identical to ${CMAKE_CURRENT_SOURCE_DIR}).

  cc_get_optional_keyword(CONFIGURE_DEPENDS "${arg_CONFIGURE_DEPENDS}")

  foreach(dir ${arg_DIRECTORIES})
    cmake_path(ABSOLUTE_PATH dir NORMALIZE
      OUTPUT_VARIABLE abs_dir)

    cmake_path(GET abs_dir
      PARENT_PATH anchor_dir)

    list(TRANSFORM arg_FILENAME_PATTERN
      PREPEND "${abs_dir}/"
      OUTPUT_VARIABLE filename_patterns)

    file(GLOB_RECURSE rel_paths
      RELATIVE "${anchor_dir}"
      LIST_DIRECTORIES FALSE
      ${CONFIGURE_DEPENDS}
      ${filename_patterns})

    foreach(rel_path ${rel_paths})
      list(APPEND sources "${anchor_dir}/${rel_path}")
      list(APPEND outputs "${arg_OUTPUT_DIR}/${rel_path}")
    endforeach()
  endforeach()

  if(arg_SOURCES_VARIABLE)
    set("${arg_SOURCES_VARIABLE}" "${sources}" PARENT_SCOPE)
  endif()

  if(arg_OUTPUTS_VARIABLE)
    set("${arg_OUTPUTS_VARIABLE}" "${outputs}" PARENT_SCOPE)
  endif()
endfunction()
