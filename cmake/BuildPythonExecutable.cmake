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
  set(_options DEBUG INFO DIRECTORY_BUNDLE)
  set(_singleargs SCRIPT PROGRAM TYPE INSTALL_COMPONENT PYTHON_INTERPRETER VENV)
  set(_multiargs BUILD_DEPS PYTHON_DEPS PYINSTALLER_EXTRA_ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Do this only if the option `BUILD_PYTHON_EXECUTABLE` is enabled
  if(NOT BUILD_PYTHON_EXECUTABLE)
    return()
  endif()

  ### Determine Python interpreter based on PYTHON_INTERPRETER or VENV
  if(arg_PYTHON_INTERPRETER)
    set(python "${arg_PYTHON_INTERPRETER}")
  elseif(arg_VENV)
    set(python "${arg_VENV}/bin/python")
  elseif(PYTHON_VENV)
    set(python "${PYTHON_VENV}/bin/python")
  else()
    message(FATAL_ERRROR "BuildPythonExecutable(${TARGET}) requires PYTHON_INTERPRETER or VENV")
  endif()

  ### Determine name of executable from PROGRAM or else TARGET
  if(arg_PROGRAM)
    set(program "${arg_PROGRAM}")
  else()
    set(program "${TARGET}")
  endif()

  if(NOT arg_PYTHON_DEPS)
    message(FATAL_ERROR "BuildPythonExecutable(${TARGET}) requires PYTHON_DEPS")
  endif()

  ### Specify a working directory for PyIntaller
  set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/pyinstaller/${program}")
  set(workdir "${out_dir}/workdir")
  set(distdir "${out_dir}/dist")
  set(program_path "${distdir}/${program}")

  ### Create a CMake target
  add_custom_target("${TARGET}" ALL DEPENDS "${program_path}")
  add_dependencies("${TARGET}" ${arg_BUILD_DEPS} ${arg_PYTHON_DEPS})

  ### Copy sources from the specified target dependencies into a consolidated
  ### staging area.  This is needed because `PyInstaller` cannot handle
  ### multiple source paths with overlappiing directoreis/namespaces.
  set(staging_dir "${out_dir}/staging")
  get_target_property_recursively(staging_dir
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE dep_staging_dirs
    REQUIRED)

  message(DEBUG
    "Creating PyInstaller staging directory ${staging_dir} from: ${dep_staging_dirs}")

  add_custom_command(
    OUTPUT "${program_path}"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${dep_staging_dirs} "${staging_dir}"
    COMMENT "Building PyIntaller executable: ${program}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )


  ### Construct arguments for PyInstaller
  set(pyinstall_args)
  if(arg_DEBUG)
    list(APPEND pyinstall_args "--log-level" "DEBUG")
  elseif(NOT arg_INFO)
    list(APPEND pyinstall_args "--log-level" "WARN")
  endif()

  if(arg_DIRECTORY_BUNDLE)
    list(APPEND pyinstall_args "--onedir")
  else()
    list(APPEND pyinstall_args "--onefile")
  endif()

  list(APPEND pyinstall_args "--paths" "${staging_dir}")
  list(APPEND pyinstall_args ${arg_PYINSTALL_EXTRA_ARGS})

  message(DEBUG
    "Creating PyInstaller executable: ${program} ${TARGET} from ${arg_PYTHON_DEPS} outputs: ${outputs}")

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${program_path}"
    DEPENDS "${arg_SCRIPT}" "${arg_PYTHON_DEPS}"
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
    COMMENT "Building PyInstaller executable: ${program}"
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    APPEND
  )

  ### Install/package resulting executable
  if(arg_INSTALL_COMPONENT)
    if(arg_TYPE)
      set(type "${arg_TYPE}")
    else()
      set(type BIN)
    endif()

    if(arg_DIRECTORY_BUNDLE)
      install(
        DIRECTORY "${program_path}"
        TYPE "${type}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
        USE_SOURCE_PERMISSIONS
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
