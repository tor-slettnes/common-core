## -*- cmake -*-
#===============================================================================
## @file cpack_pyinstall.cmake
## @brief Create executables Python virtual environments using PyInstall
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


#===============================================================================
## @fn GET_COMPONENT_DIR
## @brief Get CPack staging folder for the specified package component

function(GET_COMPONENT_DIR COMPONENT OUT_VAR)
  set(_dir "${CPACK_TEMPORARY_DIRECTORY}")

  if (CPACK_DEB_COMPONENT_INSTALL)
    if (CPACK_COMPONENTS_GROUPING STREQUAL "IGNORE")
      set(_dir "${_dir}/${COMPONENT}")
    elseif(CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
      string(TOUPPER "${COMPONENT}" _upper)
      set(_var "CPACK_COMPONENT_${_upper}_GROUP")
      set(_group ${${_var}})
      set(_dir "${_dir}/${_group}")
    endif()
  endif()

  set(${OUT_VAR} ${_dir} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn GET_COMPONENT_DEPENDS
## @brief Get depended CPack package components for the specified component

function(GET_COMPONENT_DEPENDS COMPONENT OUT_VAR)
  string(TOUPPER "${COMPONENT}" _upper)
  set(_var "CPACK_COMPONENT_${_upper}_DEPENDS")
  set(${OUT_VAR} ${${_var}} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn GET_COMPONENT_PYINSTALLS
## @brief
##   Get Python programs to install for the specified CPack package component

function(GET_COMPONENT_PYINSTALLS COMPONENT OUT_VAR)
  string(TOUPPER "${COMPONENT}" _upper)
  set(_var "CPACK_COMPONENT_${_upper}_PYINSTALL")
  set(${OUT_VAR} ${${_var}} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn GET_COMPONENT_BUILD_DIR
## @brief
##   Get PyInstall work directory for the specified component

function(GET_COMPONENT_BUILD_DIR COMPONENT OUT_VAR)
  string(TOUPPER "${COMPONENT}" _upper)
  set(_var "CPACK_COMPONENT_${_upper}_BUILD_DIR")
  set(${OUT_VAR} ${${_var}} PARENT_SCOPE)
endfunction()



#===============================================================================
## @fn GET_COMPONENT_DEP_DIRS
## @brief
##   Get CPack staging folder fo the specified package component and its
##   dependencies

function(GET_COMPONENT_DEP_DIRS COMPONENT OUT_VAR)
  get_component_dir("${COMPONENT}" _roots)
  get_component_depends("${COMPONENT}" _depends)

  foreach(_dependee ${_depends})
    get_component_dir("${_dependee}" _dir)
    list(APPEND _roots "${_dir}")
  endforeach()

  set(${OUT_VAR} ${_roots} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn GET_COMPONENT_PYTHON_ROOT)
## @brief Get Python root install folder for the specified component

function(GET_COMPONENT_PYTHON_ROOT COMPONENT OUT_VAR)
  get_component_dir("${COMPONENT}" _root)

  if (CPACK_PACKAGING_INSTALL_PREFIX)
    set(_install_prefix ${CPACK_PACKAGING_INSTALL_PREFIX})
  else()
    set(_install_prefix "/usr")
  endif()

  set(${OUT_VAR} "${_root}${_install_prefix}/${CPACK_PYTHON_INSTALL_DIR}"
    PARENT_SCOPE)
endfunction()


#===============================================================================
## @fn PY_INSTALL
## @brief
##   Create a Python executable with PyInstaller using the specified
##   Python virtual environment

function(PY_INSTALL)
  set(_options)
  set(_singleargs PROGRAM SCRIPT COMPONENT)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Create a working directory
  get_component_build_dir("${arg_COMPONENT}" _build_dir)
  set(_staging_dir "${_build_dir}/pyinstaller")
  file(MAKE_DIRECTORY "${_staging_dir}")

  ### Split program into directory + filename for PyInstaller
  cmake_path(GET arg_PROGRAM PARENT_PATH _program_dir)
  cmake_path(GET arg_PROGRAM FILENAME _program_name)

  ### Construct Python search path for component and its dependencies
  get_component_python_root("${_arg_COMPONENT}" _python_roots)
  get_component_deps("${arg_COMPONENT}" _deps)
  foreach(_dep ${_deps})
    get_component_python_root("${_dep}" _dir)
    list(APPEND _python_roots "${_dir}")
  endforeach()
  list(TRANSFORM _python_roots PREPEND "-p " OUTPUT_VARIABLE _path_args)

  ### Now invoke PyInstaller to create standalone executable
  execute_process(
    COMMAND /home/tslettnes/bin/testenv
      "${CPACK_PYTHON_VENV}/bin/python" -m PyInstaller
      -F
      --name "${program_name}"
      ${_path_args}
      --workpath "${_staging_dir}"
      --specpath "${_staging_dir}"
      --distpath "${_program_dir}"
#      --add-data "${arg_TARGET_ROOT}/${arg_PYTHON_ROOT}/cc/generated:cc/generated"
      "${arg_SCRIPT}"

    WORKING_DIRECTORY "${CPACK_PYTHON_VENV}"
  )
endfunction()


if (IS_DIRECTORY "${CPACK_PYTHON_VENV}")
  foreach(_component ${CPACK_COMPONENTS_ALL})
    get_component_pyinstalls(${_component} _pyinstall)
    if (_pyinstall)
      message(STATUS "Creating PyInstalls in VENV ${CPACK_PYTHON_VENV} for component ${_component}: ${_pyinstall}")

      foreach(_program_pair ${_pyinstall})
        string(REGEX_MATCH "^([^:]+):(.*)" _match "${_program_pair}")
        set(_program "${CMAKE_MATCH_1}")
        set(_script  "${CMAKE_MATCH_2}")

        message(STATUS
          "Building Python executable from "
          "program=${_program}, script=${_script}")

        py_install(
          PROGRAM "${_program}"
          SCRIPT "${_script}"
          COMPONENT "${_component}"
        )
      endforeach()
    endif()
  endforeach()
else()
  message(FATAL_ERROR "Python VENV does not exist: ${CPACK_PYTHON_VENV}")
endif()

