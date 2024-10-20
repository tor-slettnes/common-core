## -*- cmake -*-
#===============================================================================
## @file BuildPython.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(utility)

set(PYTHON_TEMPLATE_DIR
  "${CMAKE_CURRENT_LIST_DIR}/python")

set(PYTHON_INSTALL_DIR "lib/python3/dist-packages"
  CACHE STRING "Top-level installation directory for Python modules")


#===============================================================================
## @fn BuildPython
## @brief
##  Add/populate a Python target, with sources organized by namespace in a staging directory

function(BuildPython TARGET)
  set(_options)
  set(_singleargs
    NAMESPACE NAMESPACE_COMPONENT
    STAGING_DIR FILENAME_PATTERN
    INSTALL_COMPONENT INSTALL_DIR)
  set(_multiargs
    PYTHON_DEPS PROTO_DEPS
    PROGRAMS FILES DIRECTORIES
    HIDDEN_IMPORTS REQUIRED_PACKAGES
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### We populate sources into a target-specific staging directory, from where
  ### they will be:
  ###  * Picked up by `BuildPythonWheel()` and added to `pyproject.toml`
  ###  * Consolidated and staged to a common folder by `BuildPythonExecutable()`
  ###    (because PyInstaller does not handle multiple source locations with
  ###    overlapping module namespaces / directory structures)
  ###  * Installed, if the option `INSTALL_PYTHON_MODDULES` is enabled

  ### Specify root folder for staging python modules for this target
  if(arg_STAGING_DIR)
    cmake_path(APPEND "${CMAKE_CURRENT_BINARY_DIR}" "${arg_STAGING_DIR}"
      OUTPUT_VARIABLE staging_dir)
  else()
     #set(staging_dir "${CMAKE_BINARY_DIR}/python-staging")
     set(staging_dir "${CMAKE_CURRENT_BINARY_DIR}/staging")
  endif()

  ### Clean staging directory (this happens immediately when (re)configuring).
  file(REMOVE_RECURSE "${staging_dir}")

  ### Construct namespace for Python modules
  get_namespace_dir(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${arg_NAMESPACE_COMPONENT}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
    ROOT_DIR "${staging_dir}"
    OUTPUT_VARIABLE namespace_dir)

  ### Add commands to populate staging directory
  get_value_or_default(
    filename_pattern
    arg_FILENAME_PATTERN
    "*.py")

  get_python_modules(
    FILES ${arg_PROGRAMS} ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES}
    FILENAME_PATTERN "${filename_pattern}"
    OUTPUT_DIR "${namespace_dir}"
    SOURCES_VARIABLE sources
    OUTPUTS_VARIABLE staged_outputs)

  # set(staged_outputs "${namespace_dir}")
  # set_property(SOURCE "${staged_output}" PROPERTY SYMBOLIC)

  stage_python_modules(
    TARGET "${TARGET}"
    OUTPUT "${staged_outputs}"
    MODULES_DIR "${namespace_dir}"
    PROGRAMS ${arg_PROGRAMS}
    FILES ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES}
    FILENAME_PATTERN ${filename_pattern}
    SOURCES_VARIABLE sources
  )

  ### Create a Custom CMake target plus staging folder
  if(NOT TARGET "${TARGET}")
    add_custom_target("${TARGET}" ALL
      DEPENDS ${staged_outputs}
    )
  endif()

  target_sources(${TARGET} PRIVATE ${staged_outputs})

  if(arg_PYTHON_DEPS OR arg_PROTO_DEPS)
    add_dependencies("${TARGET}" ${arg_PYTHON_DEPS} ${arg_PROTO_DEPS})
  endif()


  ### Set target property `staging_dir` for downstream targets
  ### (e.g. via `BuildPythonExecutable()`)
  set_target_properties("${TARGET}"
    PROPERTIES staging_dir "${staging_dir}")

  if(arg_HIDDEN_IMPORTS)
    set_target_properties("${TARGET}"
      PROPERTIES hidden_imports ${arg_HIDDEN_IMPORTS})
  endif()

  if(arg_REQUIRED_PACKAGES)
    set_target_properties("${TARGET}"
      PROPERTIES required_packages ${arg_REQUIRED_PACKAGES})
  endif()

  ### Install source modules locally or via CPack
  message(DEBUG "BuildPython(${TARGET})"
    " - staging_dir='${staging_dir}'"
    " - INSTALL_PYTHON_MODULES='${INSTALL_PYTHON_MODULES}'"
    " - INSTALL_COMPONENT='${arg_INSTALL_COMPONENT}'")

  if(INSTALL_PYTHON_MODULES AND arg_INSTALL_COMPONENT)
    if(arg_INSTALL_DIR)
      set(_install_dir "${arg_INSTALL_DIR}")
    else()
      set(_install_dir "${PYTHON_INSTALL_DIR}")
    endif()

    install(
      DIRECTORY "${staging_dir}/"
      DESTINATION "${_install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()
endfunction()



#===============================================================================
## @fn get_python_modules
## @brief Get a recursive list of python modules given the specified targets

function(get_python_modules)
  set(_options)
  set(_singleargs OUTPUT_DIR FILENAME_PATTERN SOURCES_VARIABLE OUTPUTS_VARIABLE)
  set(_multiargs FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(sources)
  set(outputs)

  foreach(path ${arg_FILES})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${path}"
      OUTPUT_VARIABLE abs_source)
    list(APPEND sources "${abs_source}")

    cmake_path(GET path FILENAME filename)
    list(APPEND outputs "${arg_OUTPUT_DIR}/${filename}")
  endforeach()

  ### We process each directory separately in case they are not located
  ### directly below ${CMAKE_CURRENT_SOURCE_DIR}. Paths are then constructed
  ### as relative to the parent folder of each directory (${anchor_dir}, which
  ### may or may not be identical to ${CMAKE_CURRENT_SOURCE_DIR}).

  foreach(dir ${arg_DIRECTORIES})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${dir}"
      OUTPUT_VARIABLE abs_dir)

    cmake_path(GET abs_dir PARENT_PATH anchor_dir)

    file(GLOB_RECURSE rel_paths
      RELATIVE "${anchor_dir}"
      LIST_DIRECTORIES FALSE
      CONFIGURE_DEPENDS  # Trigger reconfiguration on new/deleted files
      "${abs_dir}/${arg_FILENAME_PATTERN}")

    foreach(rel_path ${rel_paths})
      list(APPEND sources "${anchor_dir}/${rel_path}")
      list(APPEND outputs "${arg_OUTPUT_DIR}/${rel_path}")
    endforeach()
  endforeach()

  if(arg_SOURCES_VARIABLE)
    set("${arg_SOURCES_VARIABLE}" "${sources}" PARENT_SCOPE)
  endif()

  if(arg_OUTPUTS_VARIABLE)
    set("${arg_OUTPUTS_VARIABLE}" "${outputs}" PARENT_SCOPE)
  endif()

endfunction()


#===============================================================================
## @fn stage_python_moduls
## @brief Populate a staging folder
##
## Instead of installing Python modules directly, we copy them to a
## target-specific staging folder from where they will be
##  - Added to dependent Python Wheel targets (via `BuildPythonWheel()`)
##  - Merged alongside other dependencies into a single staging
##    folder for Pyinstaller (via `BuildPythonExecutable()`)
##  - Installed/packaged (if the option `INSTALL_PYTHON_MODULES` is enabled)

function(stage_python_modules)
  set(_options)
  set(_singleargs TARGET MODULES_DIR FILENAME_PATTERN SOURCES_VARIABLE)
  set(_multiargs OUTPUT PROGRAMS FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Now define the first of several commands that will populate the output folder.

  add_custom_command(
    OUTPUT ${arg_OUTPUT}
    COMMENT "Staging Python modules for target ${arg_TARGET} in ${arg_MODULES_DIR}"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E rm -rf ${arg_MODULES_DIR}
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
    list(APPEND sources ${arg_PROGRAMS})
  endif()

  if(arg_FILES)
    add_custom_command(
      OUTPUT ${arg_OUTPUT} APPEND
      DEPENDS ${arg_FILES}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${arg_FILES} ${arg_MODULES_DIR}
    )
    list(APPEND sources ${arg_FILES})
  endif()

  foreach(dir ${arg_DIRECTORIES})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${dir}"
      OUTPUT_VARIABLE abs_dir)

    cmake_path(GET abs_dir PARENT_PATH anchor_dir)

    file(GLOB_RECURSE rel_paths
      RELATIVE "${anchor_dir}"
      LIST_DIRECTORIES FALSE
      CONFIGURE_DEPENDS         # Regenerate cache on new/deleted files
      "${abs_dir}/${arg_FILENAME_PATTERN}")

    foreach(rel_path ${rel_paths})
      add_custom_command(
        OUTPUT ${arg_OUTPUT} APPEND
        DEPENDS ${anchor_dir}/${rel_path}
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${anchor_dir}/${rel_path} ${arg_MODULES_DIR}/${rel_path}
      )
      list(APPEND sources "${anchor_dir}/${rel_path}")
    endforeach()
  endforeach()

  if(arg_SOURCES_VARIABLE)
    set("${arg_SOURCES_VARIABLE}" "${sources}" PARENT_SCOPE)
  endif()
endfunction()



#===============================================================================
## @fn GET_NAMESPACE_DIR
## @brief Helper function to construct Python namespace folder

function(GET_NAMESPACE_DIR)
  set(_options)
  set(_singleargs ROOT_DIR NAMESPACE NAMESPACE_COMPONENT OUTPUT_VARIABLE)
  set(_multiargs MISSING_VALUES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  get_namespace(
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
## @fn GET_NAMESPACE
## @brief Helper function to construct Python namespace folder

function(GET_NAMESPACE)
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

