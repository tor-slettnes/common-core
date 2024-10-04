## -*- cmake -*-
#===============================================================================
## @file BuildPythonExecutable.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#===============================================================================
## @fn BuildPythonExecutable
## @brief
##  Build a self-contained Python executable with PyInstaller

function(BuildPythonExecutable TARGET)
  set(_options DEBUG DIRECTORY_BUNDLE)
  set(_singleargs SCRIPT PROGRAM TYPE INSTALL_COMPONENT PYTHON_INTERPRETER VENV)
  set(_multiargs PYTHON_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Determine Python interpreter based on PYTHON_INTERPRETER or VENV
  if (arg_PYTHON_INTERPRETER)
    set(_python "${arg_PYTHON_INTERPRETER}")
  elseif(arg_VENV)
    set(_python "${arg_VENV}/bin/python")
  else()
    message(FATAL_ERRROR "AddPythonExecutable() requires PYTHON_INTERPRETER or VENV")
  endif()

  ### Determine name of executable from PROGRAM or else TARGET
  if (arg_PROGRAM)
    set(_program "${arg_PROGRAM}")
  else()
    set(_program "${TARGET}")
  endif()

  ### Specify a working directory for PyIntaller
  set(_workdir "${CMAKE_CURRENT_BINARY_DIR}/pyinstaller/${_program}")
  set(_distdir "${CMAKE_CURRENT_BINARY_DIR}/dist")

  set(_program_path ${_distdir}/${_program})

  ### Create a CMake target
  add_custom_target("${TARGET}" ALL DEPENDS "${_program_path}")

  set(pyinstall_args)
  if(arg_DEBUG)
    list(APPEND pyinstall_args "--log-level" "DEBUG")
  endif()

  if(arg_DIRECTORY_BUNDLE)
    list(APPEND pyinstall_args "--onedir")
  else()
    list(APPEND pyinstall_args "--onefile")
  endif()


  if(arg_PYTHON_DEPS)
    add_dependencies("${TARGET}" "${arg_PYTHON_DEPS}")

    ### Construct search path for `PyInstaller`
    cascade_inherited_property(
      PROPERTY python_paths
      OUTPUT_VARIABLE python_paths
      DEPENDENCIES ${arg_PYTHON_DEPS}
    )

    foreach(_path ${python_paths})
      list(APPEND pyinstall_args "-p" "${_path}")
    endforeach()

    ### Construct additional data directories
    cascade_inherited_property(
      PROPERTY proto_paths
      OUTPUT_VARIABLE proto_paths
      DEPENDENCIES ${arg_PYTHON_DEPS}
    )
    foreach (_path ${proto_paths})
      list(APPEND pyinstall_args "--add-data" "${_path}/.:.")
    endforeach()
  endif()

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${_program_path}"
    DEPENDS "${arg_SCRIPT}"
    COMMAND /home/tslettnes/bin/testenv
    ARGS  "${_python}" -m PyInstaller
      --clean --noconfirm
      ${pyinstall_args}
      --name "${_program}"
      --distpath "${_distdir}"
      --workpath "${_workdir}"
      --specpath "${_workdir}"
      --collect-submodules cc.generated
      --collect-submodules grpc
      "${arg_SCRIPT}"
    COMMAND_EXPAND_LISTS
    COMMENT "Building Python executable: ${_program}"
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )


  if (arg_INSTALL_COMPONENT)
    if(arg_TYPE)
      set(_type "${arg_TYPE}")
    else()
      set(_type BIN)
    endif()

    if (IS_DIRECTORY "${_program_path}")
      install(
        DIRECTORY "${_program_path}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
        TYPE ${_type}
        USE_SOURCE_PERMISSIONS
      )
    else()
      install(
        PROGRAMS "${_program_path}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
        TYPE ${_type}
      )
    endif()

  endif()

endfunction()
