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
  set(_options INSTALL)
  set(_singleargs NAMESPACE NAMESPACE_COMPONENT STAGING_DIR INSTALL_COMPONENT INSTALL_DIR)
  set(_multiargs
    PYTHON_DEPS PROTO_DEPS
    PROGRAMS FILES DIRECTORIES FILENAME_PATTERN
    HIDDEN_IMPORTS REQUIRED_SUBMODULES
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Create a Custom CMake target plus staging folder
  if(NOT TARGET "${TARGET}")
    add_custom_target("${TARGET}" ALL)
  endif()

  if(arg_PYTHON_DEPS OR arg_PROTO_DEPS)
    add_dependencies("${TARGET}" ${arg_PYTHON_DEPS} ${arg_PROTO_DEPS})
  endif()

  target_sources("${TARGET}" PRIVATE "${arg_PROGRAMS}" "${arg_FILES}" "${arg_DIRECTORIES}")

  ### Construct namespace for Python modules
  get_namespace_dir(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${arg_NAMESPACE_COMPONENT}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
    OUTPUT_VARIABLE namespace_dir)

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

  populate_staging_dir("${staging_dir}"
    NAMESPACE_DIR "${namespace_dir}"
    PROGRAMS ${arg_PROGRAMS}
    FILES ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES}
    FILENAME_PATTERN ${arg_FILENAME_PATTERN}
  )

  ### Set target property `staging_dir` for downstream targets
  ### (e.g. via `BuildPythonExecutable()`)
  set_target_properties("${TARGET}"
    PROPERTIES staging_dir "${staging_dir}")

  if(arg_HIDDEN_IMPORTS)
    set_target_properties("${TARGET}"
      PROPERTIES hidden_imports ${arg_HIDDEN_IMPORTS})
  endif()

  if(arg_REQUIRED_SUBMODULES)
    set_target_properties("${TARGET}"
      PROPERTIES required_submodules ${arg_REQUIRED_SUBMODULES})
  endif()

  ### Install source modules locally or via CPack
  if((arg_INSTALL OR INSTALL_PYTHON_MODULES)
      AND arg_INSTALL_COMPONENT)

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
## @fn POPULATE_STAGING_DIR
## @brief Populate a staging folder for `PyInstaller`.

function(POPULATE_STAGING_DIR STAGING_DIR)
  set(_options)
  set(_singleargs NAMESPACE_DIR)
  set(_multiargs PROGRAMS FILES DIRECTORIES FILENAME_PATTERN)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cmake_path(APPEND STAGING_DIR "${arg_NAMESPACE_DIR}"
    OUTPUT_VARIABLE modules_dir)

  ### Copy programs, files and directories to a staging subfolder for PyInstaller
  file(MAKE_DIRECTORY "${modules_dir}")

  if(arg_PROGRAMS)
    file(COPY ${arg_PROGRAMS}
      DESTINATION "${modules_dir}"
      FILE_PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE)
  endif()

  if(arg_FILES)
    file(COPY ${arg_FILES}
      DESTINATION "${modules_dir}")
  endif()

  if(arg_DIRECTORIES)
    if(arg_FILENAME_PATTERN)
      list(TRANSFORM arg_FILENAME_PATTERN PREPEND "PATTERN;"
        OUTPUT_VARIABLE match_expr)

      file(COPY ${arg_DIRECTORIES}
        DESTINATION "${modules_dir}"
        FILES_MATCHING ${match_expr})

    else()
      file(COPY ${arg_DIRECTORIES}
        DESTINATION "${modules_dir}"
        PATTERN __pycache__ EXCLUDE)
    endif()
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

