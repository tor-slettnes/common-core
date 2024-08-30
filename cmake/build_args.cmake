#===============================================================================
## @file build_args.cmake
## @brief Get values from CMake build arguments or defaults file
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(get_build_arg OUT_VAR)
  if(NOT ${OUT_VAR})
    if(NOT DEFAULTS AND EXISTS "${CMAKE_SOURCE_DIR}/defaults.json")
      file(STRINGS "${CMAKE_SOURCE_DIR}/defaults.json" DEFAULTS NEWLINE_CONSUME)
      set(DEFAULTS "${DEFAULTS}" PARENT_SCOPE)
    endif()

    string(JSON _value ERROR_VARIABLE _error GET "${DEFAULTS}" ${ARGN})
    if(_error STREQUAL "NOTFOUND")
      set("${OUT_VAR}" "${_value}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(get_build_number OUT_VAR)
  if(NOT ${OUT_VAR})
    math(EXPR _result "${LAST_BUILD}+1")
    set(LAST_BUILD "${_result}"
      CACHE INTERNAL "..." FORCE)
    set("${OUT_VAR}" "${_result}" PARENT_SCOPE)
  endif()
endfunction()
