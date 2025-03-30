## -*- cmake -*-
#===============================================================================
## @file cc_add_python.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(utility)

file(REAL_PATH "../python" PYTHON_TEMPLATE_DIR
  BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

set(PYTHON_OUT_DIR
  "${CMAKE_BINARY_DIR}/python")

set(PYTHON_STAGING_ROOT
  "${PYTHON_OUT_DIR}/staging")

set(PYTHON_VENV
  "${PYTHON_OUT_DIR}/venv"
  CACHE STRING "Python virtual environment for building wheels & executables")

if(NOT IS_ABSOLUTE PYTHON_VENV)
  cmake_path(ABSOLUTE_PATH PYTHON_VENV
    BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PYTHON_VENV)
endif()

### Add the above directories to the global `clean` target
set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_STAGING_ROOT}
)


#===============================================================================
## @fn cc_add_python
## @brief
##  Add/populate a Python target, with sources organized by namespace in a staging directory

function(cc_add_python TARGET)
  set(_options)
  set(_singleargs
    NAMESPACE                   # Override top-level namespace (default: `cc`)
    NAMESPACE_COMPONENT         # 2nd level namespace (following `cc.`)
    STAGING_DIR                 # Where to stage Python modules for installation
    INSTALL_COMPONENT           # CPack component (e.g. Debian package component)
    INSTALL_DIR                 # Override installation folder on target
  )
  set(_multiargs
    PYTHON_DEPS                 # Other Python targets on which this depends
    PROTO_DEPS                  # Generated ProtoBuf targets on which this depends
    DATA_DEPS                   # Data file (e.g. settings) targets on which this depends
    PROGRAMS                    # Files installed with execute permissions
    FILES                       # Files installed without execute permissions
    DIRECTORIES                 # Subdirectories to install. Use trailing `/` to start within.
    FILENAME_PATTERN            # Filename masks for DIRECTORIES option
    HIDDEN_IMPORTS              # Passed on to any dependent PyInstaller/executable target
    COLLECT_SUBMODULES          # Passed on to any dependent PyInstaller/executable target
    COLLECT_PACKAGES            # Passed on to any dependent PyInstaller/executable target
    EXTRA_DATA_MODULES          # Passed on to any dependent PyInstaller/executable target
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### We populate sources into a target-specific staging directory, from where
  ### they will be:
  ###  * Picked up by `cc_add_python_wheel()` and added to `pyproject.toml`
  ###  * Consolidated and staged to a common folder by `cc_add_python_executable()`
  ###    (because PyInstaller does not handle multiple source locations with
  ###    overlapping module namespaces / directory structures)
  ###  * Installed, if the option `WITH_PYTHON_MODDULES` is enabled

  ### Specify root folder for staging python modules for this target
  if(arg_STAGING_DIR)
    cmake_path(APPEND "${CMAKE_CURRENT_BINARY_DIR}" "${arg_STAGING_DIR}"
      OUTPUT_VARIABLE staging_dir)
  else()
     set(staging_dir "${PYTHON_STAGING_ROOT}/${TARGET}")
  endif()

  ### Clean staging directory (this happens immediately when (re)configuring).
  file(REMOVE_RECURSE "${staging_dir}")

  ### Construct namespace for Python modules
  cc_get_namespace(namespace
    "${arg_NAMESPACE}"
    "${arg_NAMESPACE_COMPONENT}"
    "${arg_KEYWORDS_MISSING_VALUES}")

  cc_get_namespace_dir(
    NAMESPACE "${namespace}"
    ROOT_DIR "${staging_dir}"
    OUTPUT_VARIABLE namespace_dir)

  ### Add commands to populate staging directory
  cc_get_value_or_default(
    filename_pattern
    arg_FILENAME_PATTERN
    "*.py")

  cc_get_staging_list(
    FILES ${arg_PROGRAMS} ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES}
    FILENAME_PATTERN ${filename_pattern}
    OUTPUT_DIR "${namespace_dir}"
    SOURCES_VARIABLE sources
    OUTPUTS_VARIABLE staged_outputs
  )

  # set(staged_outputs "${staging_dir}")
  # set_property(SOURCE "${staged_output}" PROPERTY SYMBOLIC)

  cc_stage_python_modules(
    TARGET "${TARGET}"
    OUTPUT "${staged_outputs}"
    MODULES_DIR "${namespace_dir}"
    PROGRAMS ${arg_PROGRAMS}
    FILES ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES}
    FILENAME_PATTERN ${filename_pattern}
  )

  if(arg_INSTALL_COMPONENT AND WITH_PYTHON_MODULES)
    set(install_component ${arg_INSTALL_COMPONENT})
  else()
    unset(install_component)
  endif()


  ### Create a Custom CMake target plus staging folder
  if(NOT TARGET "${TARGET}")
    ### We include this in the 'ALL` target iff we expect to install it.
    ### In other cases (e.g. if including this target in a Python wheel), this
    ### should be an explicit dependency for one or more downstream targets.
    cc_get_optional_keyword(ALL install_component)

    add_custom_target("${TARGET}" ${ALL}
      DEPENDS ${staged_outputs}
    )
  endif()

  ### Populate `SOURCES` property for downstream dependents (It's marked
  ### `PRIVATE` because a custom target cannot have INTERFACE or PUBLIC sources;
  ### however still available by looking up the `SOURCES` property explicitly).
  target_sources(${TARGET} PRIVATE ${staged_outputs})

  if(arg_PYTHON_DEPS OR arg_PROTO_DEPS OR arg_DATA_DEPS)
    list(TRANSFORM arg_PROTO_DEPS APPEND "_py")
    add_dependencies("${TARGET}" ${arg_PYTHON_DEPS} ${arg_PROTO_DEPS} ${arg_DATA_DEPS})
  endif()


  ### Set target property `staging_dir` for downstream targets
  ### (e.g. via `cc_add_python_executable()`)
  set_target_properties("${TARGET}"  PROPERTIES
    staging_dir "${staging_dir}"
  )

  if(arg_HIDDEN_IMPORTS)
    set_target_properties("${TARGET}"
      PROPERTIES hidden_imports "${arg_HIDDEN_IMPORTS}")
  endif()

  if(arg_COLLECT_SUBMODULES)
    set_target_properties("${TARGET}"
      PROPERTIES collect_submodules "${arg_COLLECT_SUBMODULES}")
  endif()

  if(arg_COLLECT_PACKAGES)
    set_target_properties("${TARGET}"
      PROPERTIES collect_packages "${arg_COLLECT_PACKAGES}")
  endif()

  if(arg_EXTRA_DATA_MODULES)
    set_target_properties("${TARGET}"
      PROPERTIES extra_data_modules "${arg_EXTRA_DATA_MODULES}")
  endif()

  if(install_component)
    cc_get_value_or_default(
      install_dir
      arg_INSTALL_DIR
      "${PYTHON_INSTALL_DIR}")

    install(
      DIRECTORY "${staging_dir}/"
      DESTINATION "${install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()
endfunction()


#===============================================================================
## @fn stage_python_modules
## @brief Populate a staging folder
##
## Instead of installing Python modules directly, we copy them to a
## target-specific staging folder from where they will be
##  - Added to dependent Python Wheel targets (via `cc_add_python_wheel()`)
##  - Merged alongside other dependencies into a single staging
##    folder for Pyinstaller (via `cc_add_python_executable()`)
##  - Installed/packaged (if the option `WITH_PYTHON_MODULES` is enabled)

function(cc_stage_python_modules)
  set(_options)
  set(_singleargs TARGET MODULES_DIR)
  set(_multiargs OUTPUT PROGRAMS FILES DIRECTORIES FILENAME_PATTERN)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Now define the first of several commands that will populate the output folder.

  add_custom_command(
    OUTPUT ${arg_OUTPUT}
    COMMENT ""
    BYPRODUCTS ${arg_MODULES_DIR}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${arg_MODULES_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  foreach(dir ${arg_DIRECTORIES})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${dir}"
      OUTPUT_VARIABLE abs_dir)

    cmake_path(GET abs_dir
      PARENT_PATH anchor_dir)

    list(TRANSFORM arg_FILENAME_PATTERN
      PREPEND "${abs_dir}/"
      OUTPUT_VARIABLE filename_pattern)

    file(GLOB_RECURSE rel_paths
      RELATIVE "${anchor_dir}"
      LIST_DIRECTORIES FALSE
      CONFIGURE_DEPENDS         # Regenerate cache on new/deleted files
      ${filename_pattern})

    foreach(rel_path ${rel_paths})
      add_custom_command(
        OUTPUT ${arg_OUTPUT} APPEND
        DEPENDS ${anchor_dir}/${rel_path}
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${anchor_dir}/${rel_path} ${arg_MODULES_DIR}/${rel_path}
      )
    endforeach()
  endforeach()

  if(arg_FILES)
    add_custom_command(
      OUTPUT ${arg_OUTPUT} APPEND
      DEPENDS ${arg_FILES}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${arg_FILES} ${arg_MODULES_DIR}
    )
  endif()

  if(arg_PROGRAMS)
    ### In order to set/retain executable permissions on PROGRAMS we use
    ### the external `install` command if available.
    ### Otherwise (i.e. if we're running on Windows) we simply treat PROGRAMS
    ### as regular files, and use CMake's own `copy` command.

    if(EXISTS "/usr/bin/install")
      add_custom_command(
        OUTPUT ${arg_OUTPUT} APPEND
        DEPENDS ${arg_PROGRAMS}
        COMMAND /usr/bin/install
        ARGS --mode=755 --target-directory=${arg_MODULES_DIR} ${arg_PROGRAMS}
      )
    else()
      add_custom_command(
        OUTPUT ${arg_OUTPUT} APPEND
        DEPENDS ${arg_PROGRAMS}
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${arg_PROGRAMS} ${arg_MODULES_DIR}
      )
    endif()
  endif()
endfunction()


#===============================================================================
## @fn cc_get_namespace_dir
## @brief Helper function map Python namespace to installation folder

function(cc_get_namespace_dir)
  set(_options)
  set(_singleargs NAMESPACE ROOT_DIR OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(REPLACE "." "/" namespace_dir "${arg_NAMESPACE}")
  if(arg_ROOT_DIR)
    cmake_path(APPEND arg_ROOT_DIR "${namespace_dir}"
      OUTPUT_VARIABLE namespace_dir)
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${namespace_dir}" PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_get_namespace
## @brief Helper function to construct Python namespace

function(cc_get_namespace
    OUTPUT_VARIABLE             # Variable which will be populated with result
    NAMESPACE                   # Complete namespace, replacing `cc` at root
    NAMESPACE_COMPONENT         # Alternatively: 2nd level namespace (following `cc.`)
    MISSING_VALUES              # Keyword arguments that were provided without value
  )

  cc_get_argument_or_default(namespace NAMESPACE "${PYTHON_NAMESPACE}" "${MISSING_VALUES}")

  if(NAMESPACE_COMPONENT)
    string(JOIN "." namespace ${namespace} "${NAMESPACE_COMPONENT}")
  endif()

  if(OUTPUT_VARIABLE)
    set("${OUTPUT_VARIABLE}" "${namespace}" PARENT_SCOPE)
  endif()
endfunction()

#===============================================================================
## @fn cc_find_python
## @brief Helper function to obtain Python interpreter, or die trying

function(cc_find_python)
  set(_options OPTIONAL ALLOW_SYSTEM ALLOW_DEFAULT_VENV)
  set(_singleargs ACTION PYTHON_INTERPRETRER VENV OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  unset(python)
  if(arg_PYTHON_INTERPRETER)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_PYTHON_INTERPRETER}"
      OUTPUT_VARIABLE python)

  elseif(arg_VENV)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_VENV}" "bin/python"
      OUTPUT_VARIABLE python)

  elseif(arg_ALLOW_DEFAULT_VENV AND PYTHON_VENV)
    cmake_path(APPEND PYTHON_VENV "bin/python"
      OUTPUT_VARIABLE python)

  elseif(arg_ALLOW_SYSTEM)
    cc_get_ternary(REQUIRED arg_OPTIONAL "" "REQUIRED")
    find_package(Python3
      COMPONENTS Interpreter
      ${REQUIRED}
    )

    if (Python3_Interpreter_FOUND)
      set(python "${Python3_EXECUTABLE}")

    else()
      cc_get_ternary(level arg_OPTIONAL "DEBUG" "FATAL_ERROR")
      message("${level}" "${arg_ACTION} requires a Python interpreter")
    endif()

  else()
    cc_get_ternary(level arg_OPTIONAL "DEBUG" "FATAL_ERROR")
    message("${level}" "${arg_ACTION} requires PYTHON_INTERPRETER or VENV")
  endif()

  if(arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}" "${python}" PARENT_SCOPE)
  endif()
endfunction()

