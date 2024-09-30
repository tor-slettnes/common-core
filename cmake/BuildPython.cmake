## -*- cmake -*-
#===============================================================================
## @file BuildPython.cmake
## @brief CMake include file to "build" (install) Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_INSTALL_DIR "lib/python3/dist-packages"
  CACHE STRING "Top-level installation directory for Python modules" FORCE)

function(BuildPython TARGET)
  set(_options)
  set(_singleargs DESTINATION NAMESPACE PACKAGE_COMPONENT INSTALL_COMPONENT)
  set(_multiargs PROGRAMS FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  add_library("${TARGET}" INTERFACE)

  if(arg_INSTALL_COMPONENT)
    set(_component ${arg_INSTALL_COMPONENT})
  # elseif(CPACK_CURRENT_COMPONENT)
  #   set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination ${PYTHON_INSTALL_DIR})
  endif()

  if(arg_NAMESPACE)
    set(_namespace "${arg_NAMESPACE}")
  else()
    list(FIND arg_KEYWORDS_MISSING_VALUES NAMESPACE _found)
    if(${_found} LESS 0 AND PYTHON_NAMESPACE)
      set(_namespace "${PYTHON_NAMESPACE}")
    endif()
  endif()

  if(arg_PACKAGE_COMPONENT)
    string(JOIN "." _namespace "${_namespace}" "${arg_PACKAGE_COMPONENT}")
  endif()

  if(_namespace)
    string(REPLACE "." "/" _package_dir "${_namespace}")
    set(_destination "${_destination}/${_package_dir}")
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


