## -*- cmake -*-
#===============================================================================
## @file cc_add_python_executable.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#===============================================================================
## @fn cc_add_python_executable
## @brief
##  Build a self-contained Python executable with PyInstaller

function(cc_add_python_executable TARGET)
  set(_options DEBUG INFO USE_SPEC DIRECTORY_BUNDLE)
  set(_singleargs SCRIPT PROGRAM TYPE INSTALL_COMPONENT SPEC_TEMPLATE PYTHON_INTERPRETER VENV)
  set(_multiargs BUILD_DEPS PYTHON_DEPS DATA_DEPS RUNTIME_HOOKS PYINSTALLER_EXTRA_ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Do this only if the option `BUILD_PYTHON_EXECUTABLE` is enabled
  if(NOT BUILD_PYTHON_EXECUTABLE)
    message(STATUS "Skipping PyInstaller target ${TARGET}, as BUILD_PYTHON_EXECUTABLE is disabled")
    return()
  endif()

  cc_find_python(
    ACTION "cc_add_python_executable(${TARGET})"
    PYTHON_INTERPRETER "${arg_PYTHON_INTERPRETER}"
    VENV "${arg_VENV}"
    ALLOW_DEFAULT_VENV
    OUTPUT_VARIABLE python)

  if(NOT arg_SCRIPT)
    message(FATAL_ERROR "cc_add_python_executable{${TARGET}) requires SCRIPT")
  endif()

  if(NOT arg_PYTHON_DEPS)
    message(FATAL_ERROR "cc_add_python_executable(${TARGET}) requires PYTHON_DEPS")
  endif()

  ### Determine name of executable from PROGRAM or else TARGET
  cc_get_value_or_default(program
    arg_PROGRAM
    "${TARGET}")

  ### Specify a working directory for PyIntaller
  set(out_dir "${PYTHON_OUT_DIR}/pyinstaller/${TARGET}")
  set(workdir "${out_dir}")
  set(staging_dir "${out_dir}/staging")
  set(distdir "${out_dir}/dist")

  cmake_path(APPEND distdir "${program}"
    OUTPUT_VARIABLE program_path)

  cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_SCRIPT}"
    OUTPUT_VARIABLE script)

  ### Clean it
  file(REMOVE_RECURSE "${out_dir}")
  file(MAKE_DIRECTORY "${staging_dir}")

  ### Create a CMake target
  add_custom_target("${TARGET}" ALL
    DEPENDS "${program_path}")

  if(arg_BUILD_DEPS OR arg_PYTHON_DEPS OR arg_DATA_DEPS)
    add_dependencies("${TARGET}"
      ${arg_BUILD_DEPS}
      ${arg_PYTHON_DEPS}
      ${arg_DATA_DEPS})
  endif()


  ### Collect per-target staged modules from the specified target dependencies
  ### into a consolidated staging area.  This is needed because `PyInstaller`
  ### cannot handle multiple source paths with overlappiing
  ### directories/namespaces.

  cc_get_target_property_recursively(
    PROPERTY staging_dir
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE dep_staging_dirs
    REMOVE_DUPLICATES
    REQUIRED)

  cc_get_target_property_recursively(
    PROPERTY SOURCES
    TARGETS ${TARGET}
    OUTPUT_VARIABLE sources
    REMOVE_DUPLICATES)

  message(DEBUG
    "Building target ${TARGET} executable ${program_path} with deps ${sources}")

  add_custom_command(
    OUTPUT "${program_path}"
    DEPENDS ${arg_BUILD_DEPS} ${arg_PYTHON_DEPS} ${arg_DATA_DEPS} ${sources}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${dep_staging_dirs} "${staging_dir}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    COMMENT "Building PyInstaller executable: ${program}"
    WORKING_DIRECTORY "${staging_dir}"
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

  ## Expand paths of any provided runtime hooks
  set(runtime_hooks)
  foreach(hook ${arg_RUNTIME_HOOKS})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${hook}"
      OUTPUT_VARIABLE hook_path)

    list(APPEND runtime_hooks "${hook_path}")
  endforeach()


  #-----------------------------------------------------------------------------
  ## Collect various properties from our dependencies and prepare corresponding
  ## arguments for PyInstaller.

  ## Get hidden imports (modules/packages that PyInstaller does not detect)
  cc_get_target_property_recursively(
    PROPERTY hidden_imports
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE hidden_imports
    REMOVE_DUPLICATES)

  ## Get submodules that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY collect_submodules
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE collect_submodules
    REMOVE_DUPLICATES)

  ## Get packages that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY collect_packages
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE collect_packages
    REMOVE_DUPLICATES)

  ## Get data modules that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY extra_data_modules
    TARGETS ${arg_PYTHON_DEPS}
    OUTPUT_VARIABLE extra_data_modules
    REMOVE_DUPLICATES)

  ## Get settings files and other data that should also be included in output.
  cc_get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_DATA_DEPS}
    SEPARATOR "/.:"
    OUTPUT_VARIABLE extra_data
    ALL_OR_NOTHING
    REMOVE_DUPLICATES)

  ## If we are asked to use a spec template file, define variable subtitutions
  ## and generate the coresponding `.spec` file now.
  if(arg_SPEC_TEMPLATE OR arg_USE_SPEC)
    set(EXECUTABLE_NAME "${program}")

    cc_join_quoted(script OUTPUT_VARIABLE SCRIPTS)
    cc_join_quoted(hidden_imports OUTPUT_VARIABLE HIDDEN_IMPORTS)
    cc_join_quoted(collect_submodules OUTPUT_VARIABLE COLLECT_SUBMODULES)
    cc_join_quoted(collect_packages OUTPUT_VARIABLE COLLECT_PACKAGES)
    cc_join_quoted(extra_data OUTPUT_VARIABLE EXTRA_DATA)
    cc_join_quoted(extra_data_modules OUTPUT_VARIABLE EXTRA_DATA_MODULES)
    cc_join_quoted(runtime_hooks OUTPUT_VARIABLE RUNTIME_HOOKS)

    cc_get_value_or_default(
      spec_template
      arg_SPEC_TEMPLATE
      ${PYTHON_TEMPLATE_DIR}/pyinstaller.spec.in)

    set(spec_file "${out_dir}/${program}.spec")

    configure_file(
      "${spec_template}"
      "${spec_file}")

    list(APPEND pyinstall_args "${spec_file}")

  else()
    if(extra_data_modules)
      message(WARNING "One or more Python depenencies were defined with EXTRA_DATA_MODULES; "
        "however, this can only be supported with custom logic in a PyInstaller .spec file. "
        "Consider adding `USE_SPEC` in invocation `cc_add_python_executable(${TARGET} ...)`."
      )
    endif()

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

    foreach(package ${collect_submodules})
      list(APPEND pyinstall_args "--collect-submodules" "${package}")
    endforeach()

    foreach(package ${collect_packages})
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

  list(APPEND pyinstall_args ${arg_PYINSTALLER_EXTRA_ARGS})

  message(DEBUG
    "Invoking PyInstall for target ${TARGET} with args: ${pyinstall_args}")

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${program_path}" APPEND
    COMMAND ${python}
    ARGS -m PyInstaller ${pyinstall_args}
  )

  ### Install/package resulting executable
  if(arg_INSTALL_COMPONENT)
    cc_get_value_or_default(type arg_TYPE "BIN")

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
