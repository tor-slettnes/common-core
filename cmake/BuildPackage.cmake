## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(CPackConfig VARIABLE VALUE)
    file(APPEND "${CMAKE_BINARY_DIR}/CPackConfig.cmake"
      "set(${VARIABLE} \"${VALUE}\")\n")
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

