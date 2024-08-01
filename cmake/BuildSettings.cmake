## -*- cmake -*-
#===============================================================================
## @file BuildSettings.cmake
## @brief CMake include file to "build" (install) settings files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(SETTINGS_DIR "share/cc/settings"
  CACHE STRING "Top-level installation directory for settings files" FORCE)

function(BuildSettings)
  set(_options)
  set(_singleargs DIRECTORY DESTINATION INSTALL_COMPONENT)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination "${SETTINGS_DIR}")
  endif()

  if(arg_DIRECTORY)
    set(_directory "${arg_DIRECTORY}")
  else()
    set(_directory ".")
  endif()

  if(arg_INSTALL_COMPONENT)
    set(_component ${arg_INSTALL_COMPONENT})
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
