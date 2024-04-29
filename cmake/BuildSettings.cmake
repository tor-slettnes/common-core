## -*- cmake -*-
#===============================================================================
## @file BuildSettings.cmake
## @brief CMake include file to "build" (install) settings files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(BuildSettings)
  set(_options)
  set(_singleargs DIRECTORY DESTINATION COMPONENT)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination "share/settings")
  endif()

  if(arg_DIRECTORY)
    set(_directory "${arg_DIRECTORY}")
  else()
    set(_directory ".")
  endif()

  if(arg_COMPONENT)
    set(_component ${arg_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  install(
    DIRECTORY "${_directory}"
    DESTINATION "${_destination}"
    COMPONENT "${_component}"
    FILES_MATCHING PATTERN "*.json"
  )

endfunction()
