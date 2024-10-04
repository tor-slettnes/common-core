## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(CPackConfig VARIABLE VALUE)
  set(_options APPEND)
  set(_singleargs)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(REPLACE "\"" "\\\"" _value "${VALUE}")
  string(TOUPPER "${VARIABLE}" _variable)

  if (arg_APPEND)
    set(_command "list(APPEND ${_variable} \"${_value}\")")
  else()
    set(_command "set(${_variable} \"${_value}\")")
  endif()

  file(APPEND "${CMAKE_BINARY_DIR}/CPackConfig.cmake" "${_command}\n")
endfunction()


function(BuildPackageComponent NAME)
  set(_options)
  set(_singleargs)
  set(_multiargs EXTRA_CONTROL)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_EXTRA_CONTROL)
    string(TOUPPER "${NAME}" COMPONENT)
    CPackConfig(CPACK_DEBIAN_${COMPONENT}_PACKAGE_CONTROL_EXTRA "${arg_EXTRA_CONTROL}")
  endif()

  cpack_add_component(${arg_UNPARSED_ARGUMENTS})
endfunction()

