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
  set(_options DEBUG INFO USE_SPEC DIRECTORY_BUNDLE)
  set(_singleargs SCRIPT PROGRAM TYPE INSTALL_COMPONENT SPEC_TEMPLATE PYTHON_INTERPRETER VENV)
  set(_multiargs BUILD_DEPS PYTHON_DEPS DATA_DEPS RUNTIME_HOOKS PYINSTALLER_EXTRA_ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Do this only if the option `BUILD_PYTHON_EXECUTABLE` is enabled
  if(NOT BUILD_PYTHON_EXECUTABLE)
    return()
  endif()

  ### Determine Python interpreter based on PYTHON_INTERPRETER or VENV
  if(arg_PYTHON_INTERPRETER)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_PYTHON_INTERPRETER}"
      OUTPUT_VARIABLE python)
  elseif(arg_VENV)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_VENV}" "bin/python"
      OUTPUT_VARIABLE python)
  elseif(PYTHON_VENV)
    cmake_path(APPEND CMAKE_SOURCE_DIR "${PYTHON_VENV}" "bin/python"
      OUTPUT_VARIABLE python)
  else()
    message(FATAL_ERRROR "BuildPythonExecutable(${TARGET}) requires PYTHON_INTERPRETER or VENV")
  endif()

  if(NOT arg_SCRIPT)
    message(FATAL_ERROR "BuildPythonExecutable{${TARGET}) requires SCRIPT")
  endif()

  if(NOT arg_PYTHON_DEPS)
    message(FATAL_ERROR "BuildPythonExecutable(${TARGET}) requires PYTHON_DEPS")
  endif()

  ### Determine name of executable from PROGRAM or else TARGET
  get_value_or_default(program
    arg_PROGRAM
    "${TARGET}")

  ### Specify a working directory for PyIntaller
  set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/pyinstaller/${TARGET}")
  set(staging_dir "${out_dir}/staging")
  set(workdir "${out_dir}/workdir")
  set(distdir "${out_dir}/dist")

  cmake_path(APPEND distdir "${program}"
    OUTPUT_VARIABLE program_path)

  cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_SCRIPT}"
    OUTPUT_VARIABLE script)

  ### Clean it
  file(REMOVE_RECURSE "${out_dir}")

  ### Create a CMake target
  add_custom_target("${TARGET}" ALL DEPENDS "${program_path}")
  add_dependencies("${TARGET}" ${arg_BUILD_DEPS} ${arg_PYTHON_DEPS})

  ### Copy sources from the specified target dependencies into a consolidated
  ### staging area.  This is needed because `PyInstaller` cannot handle
  ### multiple source paths with overlappiing directoreis/namespaces.
  get_target_properties_recursively(
    PROPERTIES staging_dir
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE dep_staging_dirs
    REMOVE_DUPLICATES
    REQUIRED)

  set_property(SOURCE staged-${TARGET}
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
    "--noconfirm")

  if(arg_DEBUG)
    list(APPEND pyinstall_args "--log-level" "DEBUG")
  elseif(NOT arg_INFO)
    list(APPEND pyinstall_args "--log-level" "WARN")
  endif()

  list(APPEND pyinstall_args
    "--workpath" "${workdir}"
    "--distpath" "${distdir}"
  )

  #-----------------------------------------------------------------------------
  ## Collect various properties from our dependencies and set
  ## corresponding options for PyInstaller

  ### Prepare substitutions for `pyinstaller.spec.in`
  ## Get hidden imports (modules/packages that PyInstaller does not detect)
  get_target_properties_recursively(
    PROPERTIES hidden_imports
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE hidden_imports
    REMOVE_DUPLICATES)

  ## Get packages that must be collected explicitly
  get_target_properties_recursively(
    PROPERTIES required_packages
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE required_packages
    REMOVE_DUPLICATES)

  ## Get settings files and other data that should also be included in output.
  get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_DATA_DEPS}
    SEPARATOR "/.:"
    OUTPUT_VARIABLE extra_data
    ALL_OR_NOTHING
    REMOVE_DUPLICATES)

  ## Expand paths of any provided runtime hooks
  set(runtime_hooks)
  foreach(hook ${arg_RUNTIME_HOOKS})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${hook}"
      OUTPUT_VARIABLE hook_path)

    list(APPEND runtime_hooks "${hook_path}")
  endforeach()


  ## If we are asked to use a spec template file, define variable subtitutions
  ## and generate the coresponding `.spec` file now.
  if(arg_SPEC_TEMPLATE OR arg_USE_SPEC)
    set(EXECUTABLE_NAME "${program}")

    join_quoted(script OUTPUT_VARIABLE SCRIPTS)
    join_quoted(hidden_imports OUTPUT_VARIABLE HIDDEN_IMPORTS)
    join_quoted(required_packages OUTPUT_VARIABLE COLLECT_ALL)
    join_quoted(extra_data OUTPUT_VARIABLE EXTRA_DATA)
    join_quoted(runtime_hooks OUTPUT_VARIABLE RUNTIME_HOOKS)

    get_value_or_default(
      spec_template
      arg_SPEC_TEMPLATE
      ${PYTHON_TEMPLATE_DIR}/pyinstaller.spec.in)

    set(spec_file "${out_dir}/${program}.spec")

    configure_file(
      "${spec_template}"
      "${spec_file}")

    list(APPEND pyinstall_args "${spec_file}")

  else()
    list(APPEND pyinstall_args
      "--specpath" "${workdir}"
      "--name" "${program}")

    if(arg_DIRECTORY_BUNDLE)
      list(APPEND pyinstall_args "--onedir")
    else()
      list(APPEND pyinstall_args "--onefile")
    endif()

    foreach(import ${hidden_imports})
      list(APPEND pyinstall_args "--hidden-import" "${import}")
    endforeach()

    foreach(package ${required_packages})
      list(APPEND pyinstall_args "--collect-all" "${package}")
    endforeach()

    foreach(pair ${extra_data})
      list(APPEND pyinstall_args "--add-data" "${pair}")
    endforeach()

    foreach(hook ${runtime_hooks})
      list(APPEND pyinstall_args "--runtime-hook" "${hook}")
    endforeach()

    list(APPEND pyinstall_args "${script}")
  endif()

  list(APPEND pyinstall_args ${arg_PYINSTALL_EXTRA_ARGS})

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
    get_value_or_default(type arg_TYPE "BIN")

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
