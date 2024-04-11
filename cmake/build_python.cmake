## -*- cmake -*-
#===============================================================================
## @file build_python.cmake
## @brief CMake include file to "build" (install) Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(build_python)
  set(_options)
  set(_singleargs DESTINATION MODULE)
  set(_multiargs PROGRAMS FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination "share/python")
  endif()

  if(arg_MODULE)
    string(REPLACE "." "/" _module_dir "${arg_MODULE}")
    set(_destination "${_destination}/${_module_dir}")
  endif()

  if(arg_PROGRAMS)
    install(
      PROGRAMS ${arg_PROGRAMS}
      DESTINATION ${_destination})
  endif()

  if(arg_FILES)
    install(
      FILES ${arg_FILES}
      DESTINATION ${_destination})
  endif()

  if(DIRECTORIES)
    install(
      DIRECTORY ${arg_DIRECTORIES}
      DESTINATION ${_destination}
      PATTERN __pycache__ EXCLUDE)
  endif()

endfunction()
