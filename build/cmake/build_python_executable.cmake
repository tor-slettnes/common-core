## -*- cmake -*-
#===============================================================================
## @file cc_add_python_executable.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_EXECUTABLES_TARGET "python_executables")
add_custom_target(${PYTHON_EXECUTABLES_TARGET})

cmake_path(SET PYTHON_PYINSTALLER_STAGING_ROOT
  "${PYTHON_OUT_DIR}/pyinstaller")

set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_PYINSTALLER_STAGING_ROOT}
)


#===============================================================================
## @fn cc_add_python_executable
## @brief
##  Build a self-contained Python executable with PyInstaller

function(cc_add_python_executable TARGET)
  set(_options
    ALL                 # Add executable to default target
    USE_SPEC            # Use a Pyinstaller `.spec` file (see `SPEC_TEMPLATE`)
    DIRECTORY_BUNDLE    # Create a directory bundle instead of single binary
  )
  set(_singleargs
    SCRIPT              # Startup/main script (required)
    PROGRAM             # Name of executable (default: ${TARGET})
    TYPE                # CMake install type (`BIN` or `SBIN`)
    SPEC_TEMPLATE       # Use custom PyInstaller `.spec` template
    VENV_PATH           # Use existing named Python virtual environment
    VENV_DEPENDS        # Use Python venv created by `cc_add_python_venv()`
    INSTALL_CONDITION   # Boolean variable which if present controls whether to include this target
    INSTALL_COMPONENT   # CMake install component
  )
  set(_multiargs
    PYTHON_DEPS            # CMake Python target dependencies to bundle
    DATA_DEPS              # Other CMake target depenencies to bundle
    REQUIREMENTS_DEPS      # Build targets for Python distribution cache.
    REQUIREMENTS_FILES     # Override default requirements if creating new VENV
    RUNTIME_HOOKS          # Scripts to launch before main executable
    HIDDEN_IMPORTS         # Add imports not otherwise discovered py PyInstaller
    COLLECT_PACKAGES       # Additional Python packages (modules, binaries, ...)
    COLLECT_SUBMODULES     # Additional pure Python modules to import
    EXTRA_DATA             # Additional data content, in the form `source:dest_dir`.
    EXTRA_DATA_MODULES     # Additional data-only modules to import
    PYINSTALLER_EXTRA_ARGS # Extra args passed on directly to PyInstaller
  )

  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  foreach(required_arg SCRIPT PYTHON_DEPS)
    if(NOT arg_${required_arg})
      message(FATAL_ERROR "cc_add_python_executable{${TARGET}) requires ${required_arg}")
    endif()
  endforeach()

  ### Determine name of executable from PROGRAM or else TARGET
  cc_get_value_or_default(program
    arg_PROGRAM
    "${TARGET}")

  ### Specify a working directory for PyIntaller
  set(out_dir "${PYTHON_PYINSTALLER_STAGING_ROOT}/${TARGET}")
  set(workdir "${out_dir}")
  set(staging_dir "${out_dir}/staging")
  set(distdir "${out_dir}/dist")

  cmake_path(ABSOLUTE_PATH program
    BASE_DIRECTORY "${distdir}"
    OUTPUT_VARIABLE program_path)

  cmake_path(ABSOLUTE_PATH arg_SCRIPT
    BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE script)

  ### Clean it
  file(REMOVE_RECURSE "${out_dir}")
  file(MAKE_DIRECTORY "${staging_dir}")

  ### Create a CMake target
  if (arg_ALL)
    set(enable ON)
  elseif (arg_INSTALL_CONDITION)
    set(enable ${${arg_INSTALL_CONDITION}})
  elseif(arg_INSTALL_COMPONENT)
    set(enable ON)
  else()
    set(enable OFF)
  endif()

  cc_get_optional_keyword(ALL enable)

  add_custom_target(${TARGET} ${ALL}
    DEPENDS "${program_path}")
  add_dependencies(${PYTHON_EXECUTABLES_TARGET} ${TARGET})

  #-----------------------------------------------------------------------------
  # PyInstaller needs a Python virtual environment.  If we were given one, use
  # that; otherwise, add a new one.
  #
  # If we were told to use an existing VENV, we need to ensure `PyInstaller` is
  # installed into that environment. However, the same environment might be used
  # for multiple build targets, possibly running in parallel. In order to
  # prevent a race condition where multiple `pip` jobs are running
  # simultaneously, we create a commoon `${venv_dep}-pyinstaller`
  # target here, which in turn becomes a shared build dependency for each of the
  # final build targets.
  #
  # On the other hand, if we're going to create a new VENV for this job, we
  # simply add depenent target `${TARGET}-populate` to install both PyInstaller
  # and the required Python distributions.

  if(arg_VENV_DEPENDS)
    set(venv_dep "${arg_VENV_DEPENDS}")
    set(venv_populate "${venv_dep}-pyinstaller")

    if(NOT TARGET ${venv_populate})
      cc_populate_python_venv("${venv_populate}"
        VENV_TARGET "${venv_dep}"
        REQUIREMENTS "PyInstaller"
      )
    endif()

  else()
    set(venv_dep "${TARGET}-venv")
    set(venv_populate "${TARGET}-populate")

    cc_get_value_or_default(venv_path arg_VENV_PATH "${TARGET}")
    cc_add_python_venv(${venv_dep}
      VENV_PATH "${venv_path}"
      POPULATE_TARGET "${venv_populate}"
      REQUIREMENTS "PyInstaller"
      REQUIREMENTS_DEPS ${arg_REQUIREMENTS_DEPS}
      REQUIREMENTS_FILES ${arg_REQUIREMENTS_FILES}
    )
  endif()

  add_dependencies(${TARGET} ${venv_populate})


  #-----------------------------------------------------------------------------
  # Add multi-part command to build the main target, starting with obtaining the
  # `PyInstaller` tool.

  get_target_property(venv ${venv_dep} local_path)

  cmake_path(APPEND venv "bin" "python"      OUTPUT_VARIABLE python)
  cmake_path(APPEND venv ".build-stamps"     OUTPUT_VARIABLE stamp_dir)
  cmake_path(APPEND stamp_dir ${TARGET}      OUPTUT_VARIABLE target_stamp)

  message(DEBUG
    "cc_add_python_executable(${TARGET}): venv=${venv}, python=${python}")

  #-----------------------------------------------------------------------------
  # Collect per-target staged modules from the specified target dependencies
  # into a consolidated staging area.  This is needed because `PyInstaller`
  # cannot handle multiple source paths with overlappiing directory paths
  # (namespaces).

  if(arg_PYTHON_DEPS OR arg_DATA_DEPS)
    add_dependencies("${TARGET}"
      ${arg_PYTHON_DEPS}
      ${arg_DATA_DEPS})
  endif()

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
    "Adding PyInstaller target ${TARGET} executable ${program_path} with deps ${sources}")

  add_custom_command(
    OUTPUT "${program_path}"
    DEPENDS ${arg_PYTHON_DEPS} ${arg_DATA_DEPS} ${sources}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${dep_staging_dirs} "${staging_dir}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    COMMENT "${TARGET}: Building executable with PyInstaller: ${program}"
    WORKING_DIRECTORY "${staging_dir}"
  )

  ### Construct arguments for PyInstaller
  if("${CMAKE_MESSAGE_LOG_LEVEL}" STREQUAL "VERBOSE")
    set(loglevel "INFO")
  elseif("${CMAKE_MESSAGE_LOG_LEVEL}" MATCHES "^(TRACE|DEBUG)$")
    set(loglevel "DEBUG")
  else()
    set(loglevel "WARN")
  endif()

  set(pyinstall_args
    "--clean"
    "--noconfirm"
    "--log-level" "${loglevel}"
    "--workpath" "${workdir}"
    "--distpath" "${distdir}"
  )

  ## Expand paths of any provided runtime hooks
  set(runtime_hooks)
  foreach(hook ${arg_RUNTIME_HOOKS})
    cmake_path(ABSOLUTE_PATH hook
      BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    list(APPEND runtime_hooks "${hook}")
  endforeach()


  #-----------------------------------------------------------------------------
  ## Collect various properties from our dependencies and prepare corresponding
  ## arguments for PyInstaller.

  ## Get hidden imports (modules/packages that PyInstaller does not detect)
  cc_get_target_property_recursively(
    PROPERTY hidden_imports
    TARGETS ${arg_PYTHON_DEPS}
    INITIAL_VALUE ${arg_HIDDEN_IMPORTS}
    OUTPUT_VARIABLE hidden_imports
    REMOVE_DUPLICATES)

  ## Get packages that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY collect_packages
    TARGETS ${arg_PYTHON_DEPS}
    INITIAL_VALUE ${arg_COLLECT_PACKAGES}
    OUTPUT_VARIABLE collect_packages
    REMOVE_DUPLICATES)

  ## Get submodules that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY collect_submodules
    TARGETS ${arg_PYTHON_DEPS}
    INITIAL_VALUE ${arg_COLLECT_SUBMODULES}
    OUTPUT_VARIABLE collect_submodules
    REMOVE_DUPLICATES)

  ## Get data modules that must be collected explicitly
  cc_get_target_property_recursively(
    PROPERTY extra_data_modules
    TARGETS ${arg_PYTHON_DEPS}
    INITIAL_VALUE ${arg_EXTRA_DATA_MODULES}
    OUTPUT_VARIABLE extra_data_modules
    REMOVE_DUPLICATES)

  ## Get settings files and other data that should also be included in output.
  ## These may be direct target dependencies passed in via `DATA_DEPS` or
  ## indirect dependencies from `PYTHON_DEPS`, as long as they have defined both
  ## `staging_dir` and `data_dir` target properties (as in `cc_add_settings()`).
  cc_get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_PYTHON_DEPS} ${arg_DATA_DEPS}
    SEPARATOR "/.:"
    INITIAL_VALUE ${arg_EXTRA_DATA}
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

    cc_get_ternary(output_type arg_DIRECTORY_BUNDLE "onedir" "onefile")
    list(APPEND pyinstall_args "--${output_type}")

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

  message(VERBOSE
    "Invoking PyInstall for target ${TARGET} with args: ${pyinstall_args}")

  ### Rule for running PyInstaller
  add_custom_command(
    OUTPUT "${program_path}" APPEND
    COMMAND ${python}
    ARGS -m PyInstaller ${pyinstall_args}
  )

  ### Install/package resulting executable
  if(enable AND arg_INSTALL_COMPONENT)
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
