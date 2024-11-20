## -*- cmake -*-
#===============================================================================
## @file cc_add_python.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(utility)

set(PYTHON_TEMPLATE_DIR
  "${CMAKE_CURRENT_LIST_DIR}/python")

set(PYTHON_STAGING_ROOT
  "${CMAKE_BINARY_DIR}/python")

set(PYTHON_INSTALL_DIR "lib/python3/dist-packages"
  CACHE STRING "Top-level installation directory for Python modules")

### Add the above directories to the global `clean` target
set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_STAGING_ROOT} ${PYTHON_INSTALL_DIR}
)


#===============================================================================
## @fn cc_add_python
## @brief
##  Add/populate a Python target, with sources organized by namespace in a staging directory

function(cc_add_python TARGET)
  set(_options)
  set(_singleargs
    NAMESPACE NAMESPACE_COMPONENT
    STAGING_DIR
    INSTALL_COMPONENT INSTALL_DIR)
  set(_multiargs
    PYTHON_DEPS PROTO_DEPS
    PROGRAMS FILES DIRECTORIES FILENAME_PATTERN
    HIDDEN_IMPORTS COLLECT_SUBMODULES COLLECT_PACKAGES EXTRA_DATA_MODULES
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### We populate sources into a target-specific staging directory, from where
  ### they will be:
  ###  * Picked up by `cc_add_python_wheel()` and added to `pyproject.toml`
  ###  * Consolidated and staged to a common folder by `cc_add_python_executable()`
  ###    (because PyInstaller does not handle multiple source locations with
  ###    overlapping module namespaces / directory structures)
  ###  * Installed, if the option `INSTALL_PYTHON_MODDULES` is enabled

  ### Specify root folder for staging python modules for this target
  if(arg_STAGING_DIR)
    cmake_path(APPEND "${CMAKE_CURRENT_BINARY_DIR}" "${arg_STAGING_DIR}"
      OUTPUT_VARIABLE staging_dir)
  else()
     set(staging_dir "${PYTHON_STAGING_ROOT}/staging/${TARGET}")
  endif()

  ### Clean staging directory (this happens immediately when (re)configuring).
  file(REMOVE_RECURSE "${staging_dir}")

  ### Construct namespace for Python modules
  cc_get_namespace_dir(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${arg_NAMESPACE_COMPONENT}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
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

  ### Create a Custom CMake target plus staging folder
  if(NOT TARGET "${TARGET}")
    add_custom_target("${TARGET}" ALL
      DEPENDS ${staged_outputs}
    )
  endif()

  ### Populate `SOURCES` property for downstream dependents
  ### (It's marked `PRIVATE` because a custom target cannot have INTERFACE sources;
  ### however still available by looking up the `SOURCES` property explicitly).
  target_sources(${TARGET} PRIVATE ${staged_outputs})
  # target_sources(${TARGET} PRIVATE ${sources})

  if(arg_PYTHON_DEPS OR arg_PROTO_DEPS)
    add_dependencies("${TARGET}" ${arg_PYTHON_DEPS} ${arg_PROTO_DEPS})
  endif()


  ### Set target property `staging_dir` for downstream targets
  ### (e.g. via `cc_add_python_executable()`)
  set_target_properties("${TARGET}"
    PROPERTIES staging_dir "${staging_dir}")

  if(arg_HIDDEN_IMPORTS)
    set_target_properties("${TARGET}"
      PROPERTIES hidden_imports ${arg_HIDDEN_IMPORTS})
  endif()

  if(arg_COLLECT_SUBMODULES)
    set_target_properties("${TARGET}"
      PROPERTIES collect_submodules ${arg_COLLECT_SUBMODULES})
  endif()

  if(arg_COLLECT_PACKAGES)
    set_target_properties("${TARGET}"
      PROPERTIES collect_packages ${arg_COLLECT_PACKAGES})
  endif()

  if(arg_EXTRA_DATA_MODULES)
    set_target_properties("${TARGET}"
      PROPERTIES extra_data_modules ${arg_EXTRA_DATA_MUDULES})
  endif()

  if(INSTALL_PYTHON_MODULES AND arg_INSTALL_COMPONENT)
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
## @fn stage_python_moduls
## @brief Populate a staging folder
##
## Instead of installing Python modules directly, we copy them to a
## target-specific staging folder from where they will be
##  - Added to dependent Python Wheel targets (via `cc_add_python_wheel()`)
##  - Merged alongside other dependencies into a single staging
##    folder for Pyinstaller (via `cc_add_python_executable()`)
##  - Installed/packaged (if the option `INSTALL_PYTHON_MODULES` is enabled)

function(cc_stage_python_modules)
  set(_options)
  set(_singleargs TARGET MODULES_DIR)
  set(_multiargs OUTPUT PROGRAMS FILES DIRECTORIES FILENAME_PATTERN)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Now define the first of several commands that will populate the output folder.

  add_custom_command(
    OUTPUT ${arg_OUTPUT}
    COMMENT "Staging Python modules for target ${arg_TARGET}"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${arg_MODULES_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

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

  if(arg_FILES)
    add_custom_command(
      OUTPUT ${arg_OUTPUT} APPEND
      DEPENDS ${arg_FILES}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${arg_FILES} ${arg_MODULES_DIR}
    )
  endif()

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
endfunction()


#===============================================================================
## @fn cc_get_namespace_dir
## @brief Helper function to construct Python namespace folder

function(cc_get_namespace_dir)
  set(_options)
  set(_singleargs ROOT_DIR NAMESPACE NAMESPACE_COMPONENT OUTPUT_VARIABLE)
  set(_multiargs MISSING_VALUES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_namespace(
    OUTPUT_VARIABLE namespace
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${arg_NAMESPACE_COMPONENT}"
    MISSING_VALUES ${arg_MISSING_VALUES}
  )

  string(REPLACE "." "/" namespace_dir "${namespace}")

  if(arg_ROOT_DIR)
    cmake_path(APPEND arg_ROOT_DIR "${namespace_dir}"
      OUTPUT_VARIABLE namespace_dir)
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${namespace_dir}" PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_get_namespace
## @brief Helper function to construct Python namespace folder

function(cc_get_namespace)
  set(_options)
  set(_singleargs NAMESPACE NAMESPACE_COMPONENT OUTPUT_VARIABLE)
  set(_multiargs MISSING_VALUES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Construct namespace for Python modules
  if(arg_NAMESPACE)
    set(namespace "${arg_NAMESPACE}")

  else()
    list(FIND arg_MISSING_VALUES "NAMESPACE" _found)
    if(${_found} LESS 0 AND PYTHON_NAMESPACE)
      ### No 'NAMESPACE` keyword with missing/empty value.  Assign default.
      set(namespace "${PYTHON_NAMESPACE}")
    endif()

    if(arg_NAMESPACE_COMPONENT)
      string(JOIN "." namespace ${namespace} "${arg_NAMESPACE_COMPONENT}")
    endif()
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${namespace}" PARENT_SCOPE)
endfunction()
