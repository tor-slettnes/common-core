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
  set(_multiargs BUILD_DEPS PYTHON_DEPS DATA_DEPS PYINSTALLER_EXTRA_ARGS)
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

  if(NOT arg_SCRIPT)
    message(FATAL_ERROR "BuildPythonExecutable{${TARGET}) requires SCRIPT")
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
  get_target_properties_recursively(
    PROPERTIES staging_dir
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE dep_staging_dirs
    REMOVE_DUPLICATES
    REQUIRED)

  set_property(SOURCE staged_modules
    PROPERTY SYMBOLIC)

  add_custom_command(
    OUTPUT staged-${TARGET}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${dep_staging_dirs} "${staging_dir}"
    COMMENT "Staging modules for PyInstaller in ${staging_dir}."
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  ### Construct arguments for PyInstaller
  set(pyinstall_args
    "--clean"
    "--noconfirm"
    "--name" "${program}"
    "--distpath" "${distdir}"
    "--workpath" "${workdir}"
    "--specpath" "${workdir}")

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

  #-----------------------------------------------------------------------------
  ## Collect various properties from our dependencies and set
  ## corresponding options for PyInstaller

  ## Get hidden imports (modules/packages that PyInstaller does not detect)
  get_target_properties_recursively(
    PROPERTIES hidden_imports
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE hidden_imports
    REMOVE_DUPLICATES)

  foreach(import ${hidden_imports})
    list(APPEND pyinstall_args "--hidden-import" "${import}")
  endforeach()

  ## Get packages whose submodules must be collected explicitly
  get_target_properties_recursively(
    PROPERTIES required_submodules
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE required_submodules
    REMOVE_DUPLICATES)

  foreach(submodule ${required_submodules})
    list(APPEND pyinstall_args "--collect-submodules" "${submodule}")
  endforeach()

  ## Get settings files and other data that should also be included in output.
  get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_DATA_DEPS}
    SEPARATOR ":"
    OUTPUT_VARIABLE extra_data
    ALL_OR_NOTHING
    REMOVE_DUPLICATES)

  foreach(pair ${extra_data})
    string(REPLACE ":" "/.:" data_pair "${pair}")
    list(APPEND pyinstall_args "--add-data" "${data_pair}")
  endforeach()

  list(APPEND pyinstall_args ${arg_PYINSTALL_EXTRA_ARGS})

  cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_SCRIPT}"
    OUTPUT_VARIABLE script)

  list(APPEND pyinstall_args "${script}")

  message(DEBUG
    "Invoking PyInstall for target ${TARGET} with args: ${pyinstall_args}")

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${program_path}"
    DEPENDS staged-${TARGET} "${arg_PYTHON_DEPS}"
    COMMAND "${python}"
    ARGS -m PyInstaller ${pyinstall_args}
    COMMAND_EXPAND_LISTS
    COMMENT "Building PyInstaller executable: ${program}"
    VERBATIM
    WORKING_DIRECTORY "${staging_dir}"
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
