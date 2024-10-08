#===============================================================================
## @file build_args.cmake
## @brief Get values from CMake build arguments or defaults file
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(DEFAULTS_FILE "${CMAKE_SOURCE_DIR}/defaults.json"
  CACHE FILEPATH "Default build settings")

#===============================================================================
## @fn get_build_arg
## @brief
##    Look up the value of VARIABLE as a CMake or Environemnt variable,
##    with fallback to a setting from `defaults.json`.
##
## `VARIABLE` is assigned in the parent (caller's) scope in order of precedence:
##    - Its current value as a CMake or Cache variable, if available
##    - The corresponding environment variable
##    - By looking up the provided SETTING from `defaults.json`
##    - If not yet assigned and the `REQUIRED` option is set, raise an error
##    - The provided `DEFAULT` value, if any.
##
## The resulting value, if any, is assigned as a cache variable and thus
## saved for future invocations (until the CMake cache is cleaned).

function(get_build_arg VARIABLE)
  set(_options REQUIRED)
  set(_singleargs TYPE DOCSTRING DEFAULT)
  set(_multiargs SETTING)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if(NOT ${VARIABLE})
    set(value)

    if ($ENV{${VARIABLE}})
      set(value "$ENV{${VARIABLE}}")

    elseif(arg_SETTING)
      if(NOT DEFAULTS_FILE)
        message(FATAL_ERROR "Variable DEFAULTS_FILE was not defined.")

      elseif(NOT _DEFAULTS_TEXT)
        file(STRINGS "${DEFAULTS_FILE}" _DEFAULTS_TEXT NEWLINE_CONSUME)
        set(_DEFAULTS_TEXT "${_DEFAULTS_TEXT}" PARENT_SCOPE)
      endif()

      string(JSON value
        ERROR_VARIABLE _error
        GET "${_DEFAULTS_TEXT}" ${arg_SETTING})

      if(_error)
        if (_error MATCHES "not found$")
          if (arg_REQUIRED)
            message(FATAL_ERROR
              "Either ${VARIABLE} must be provided, "
              "or the setting ${setting} must be added in ${DEFAULTS_FILE}")
          else()
            set(value "${arg_DEFAULT}")
          endif()

        elseif(NOT _WARNED_DEFAULTS)
          ### JSON Parsing error (triggered only once)
          message(FATAL_ERROR "Unable to read build settings from ${DEFAULTS_FILE}: ${_error}")
          set(_WARNED_DEFAULTS TRUE PARENT_SCOPE)
        endif()
      endif()

    elseif(arg_REQUIRED)
      message(FATAL_ERROR "Build argument ${VARIABLE} is required")

    endif()

    if (value)
      if (arg_TYPE)
        set(type "${arg_TYPE}")
      else()
        set(type "STRING")
      endif()

      set("${VARIABLE}" "${value}"
        CACHE "${type}" "${arg_DOCSTRING}")
    endif()
  endif()
endfunction()

#===============================================================================
## @fn get_build_arg
## @brief
##   Get the build number as passed in;
##   otherwise incremented by one from the last build number

function(get_build_number VARIABLE)
  if(${VARIABLE})
    set(build_number "${${VARIABLE}}")

  elseif(DEFINED ENV{${VARIABLE}})
    set(build_number "$ENV{${VARIABLE}}")

  else()
    math(EXPR _result "${LAST_BUILD}+1")
    set(build_number "${_result}")
  endif()

  set(LAST_BUILD "${build_number}"
    CACHE INTERNAL "..." FORCE)

  set("${VARIABLE}" "${build_number}" PARENT_SCOPE)
endfunction()
