#===============================================================================
## @file build_args.cmake
## @brief Get values from CMake build arguments or defaults file
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(DEFAULTS_FILE "${CMAKE_SOURCE_DIR}/defaults.json"
  CACHE FILEPATH "Default build settings")

#===============================================================================
## @fn build_arg
## @brief
##    Look up the value of VARIABLE as a CMake or environemnt variable,
##    falling back to the provided DEFAULT value if not present.
##
## `VARIABLE` is assigned in the parent (caller's) scope in order of precedence:
##    - Its current value as a CMake or Cache variable, if available
##    - The corresponding environment variable
##    - The provided `DEFAULT` value.
##
## The resulting value, if any, is assigned as a cache variable and thus
## saved for future invocations (until the CMake cache is cleaned).

function(build_arg VARIABLE DEFAULT CACHE_TYPE DOCSTRING)
  set(_options)
  set(_singleargs)
  set(_multiargs CHOICES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if(NOT ${VARIABLE})
    if($ENV{${VARIABLE}})
      set(value "$ENV{${VARIABLE}}")
    else()
      set(value "${DEFAULT}")
    endif()

    set("${VARIABLE}" "${value}" CACHE "${CACHE_TYPE}" "${DOCSTRING}")

    if (arg_CHOICES)
      set_property(
        CACHE "${VARIABLE}"
        PROPERTY STRINGS "${arg_CHOICES}")
    endif()
  endif()
endfunction()

#===============================================================================
## @fn build_number
## @brief
##   Get the build number as passed in;
##   otherwise incremented by one from the last build number

function(build_number VARIABLE DOCSTRING)
  if(${VARIABLE})
    set(build_number "${${VARIABLE}}")

  elseif(DEFINED ENV{${VARIABLE}})
    set(build_number "$ENV{${VARIABLE}}")

  else()
    math(EXPR _result "${LAST_BUILD}+1")
    set(build_number "${_result}")
  endif()

  set(LAST_BUILD "${build_number}"
    CACHE STRING "${DOCSTRING}" FORCE)

  set("${VARIABLE}" "${build_number}" PARENT_SCOPE)
endfunction()
