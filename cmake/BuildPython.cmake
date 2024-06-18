## -*- cmake -*-
#===============================================================================
## @file BuildPython.cmake
## @brief CMake include file to "build" (install) Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_INSTALL_DIR "share/python"
  CACHE STRING "Top-level installation directory for Python modules" FORCE)

function(BuildPython TARGET)
  set(_options)
  set(_singleargs DESTINATION MODULE COMPONENT)
  set(_multiargs PROGRAMS FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  add_library("${TARGET}" INTERFACE)

  if(arg_COMPONENT)
    set(_component ${arg_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination ${PYTHON_INSTALL_DIR})
  endif()

  if(arg_MODULE)
    string(REPLACE "." "/" _module_dir "${arg_MODULE}")
    set(_destination "${_destination}/${_module_dir}")
  endif()

  if(arg_PROGRAMS)
    install(
      PROGRAMS ${arg_PROGRAMS}
      DESTINATION ${_destination}
      COMPONENT ${_component}
    )
  endif()

  if(arg_FILES)
    install(
      FILES ${arg_FILES}
      DESTINATION ${_destination}
      COMPONENT ${_component}
    )
  endif()

  if(arg_DIRECTORIES)
    install(
      DIRECTORY ${arg_DIRECTORIES}
      DESTINATION ${_destination}
      COMPONENT ${_component}
      PATTERN __pycache__ EXCLUDE
    )
  endif()

endfunction()


