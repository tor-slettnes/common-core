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
  set(_multiargs PYTHON_DEPS PYINSTALLER_EXTRA_ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Do this only if the option `BUILD_PYTHON_EXECUTABLE` is enabled
  if (NOT BUILD_PYTHON_EXECUTABLE)
    return()
  endif()

  ### Determine Python interpreter based on PYTHON_INTERPRETER or VENV
  if (arg_PYTHON_INTERPRETER)
    set(python "${arg_PYTHON_INTERPRETER}")
  elseif(arg_VENV)
    set(python "${arg_VENV}/bin/python")
  else()
    message(FATAL_ERRROR "BuildPythonExecutable() requires PYTHON_INTERPRETER or VENV")
  endif()

  ### Determine name of executable from PROGRAM or else TARGET
  if (arg_PROGRAM)
    set(program "${arg_PROGRAM}")
  else()
    set(program "${TARGET}")
  endif()

  ### Specify a working directory for PyIntaller
  set(workdir "${CMAKE_CURRENT_BINARY_DIR}/pyinstaller/${program}")
  set(distdir "${CMAKE_CURRENT_BINARY_DIR}/dist")

  set(program_path "${distdir}/${program}")

  ### Create a CMake target
  add_custom_target("${TARGET}" ALL DEPENDS "${program_path}")

  set(pyinstall_args)
  if(arg_DEBUG)
    list(APPEND pyinstall_args "--log-level" "DEBUG")
  endif()

  if(arg_DIRECTORY_BUNDLE)
    list(APPEND pyinstall_args "--onedir")
  else()
    list(APPEND pyinstall_args "--onefile")
  endif()

  list(APPEND pyinstall_args ${arg_PYINSTALL_EXTRA_ARGS})


  if(arg_PYTHON_DEPS)
    add_dependencies("${TARGET}" "${arg_PYTHON_DEPS}")

    ### Construct search path for `PyInstaller`
    set(paths)
    foreach(dep ${arg_PYTHON_DEPS})
      get_target_property(python_paths "${dep}" python_paths)
      if (python_paths)
        list(APPEND paths ${python_paths})
      endif()

      get_target_property(proto_paths "${dep}" proto_paths)
      if (proto_paths)
        list(APPEND paths ${proto_paths})
      endif()
    endforeach()

    foreach(path ${paths})
      list(APPEND pyinstall_args "--paths" "${path}")
    endforeach()
  endif()

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${program_path}"
    DEPENDS "${arg_SCRIPT}" "${arg_PTYHON_DEPS}"
    COMMAND "${python}"
    ARGS -m PyInstaller
      --clean --noconfirm
      ${pyinstall_args}
      --name "${program}"
      --distpath "${distdir}"
      --workpath "${workdir}"
      --specpath "${workdir}"
      "${arg_SCRIPT}"
    COMMAND_EXPAND_LISTS
    COMMENT "Building Python executable: ${program}, pyinstall_args=${pyinstall_args}"
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )


  if (arg_INSTALL_COMPONENT)
    if(arg_TYPE)
      set(type "${arg_TYPE}")
    else()
      set(type BIN)
    endif()

    if (arg_DIRECTORY_BUNDLE)
      install(
        DIRECTORY "${program_path}"
        TYPE "${type}"
        USE_SOURCE_PERMISSIONS
        COMPONENT "${arg_INSTALL_COMPONENT}"
        PATTERN "*${program}*" PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
      )
    else()
      install(
        PROGRAMS "${program_path}"
        TYPE "${type}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
      )
    endif()

  endif()

endfunction()
