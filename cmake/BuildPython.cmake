## -*- cmake -*-
#===============================================================================
## @file BuildPython.cmake
## @brief Build an executable file from a PIP deployment with PyInstaller
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_INSTALL_DIR "lib/python3/dist-packages"
  CACHE STRING "Top-level installation directory for Python modules" FORCE)

#===============================================================================
## @fn BuildPython
## @brief
##  Add/populate a Python target, with sources organized by namespace in a staging directory

function(BuildPython TARGET)
  set(_options INSTALL)
  set(_singleargs NAMESPACE NAMESPACE_COMPONENT STAGING_DIR INSTALL_COMPONENT INSTALL_DIR)
  set(_multiargs PYTHON_DEPS PROTO_DEPS PROGRAMS FILES DIRECTORIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Create a Custom CMake target plus staging folder
  if (NOT TARGET "${TARGET}")
    add_custom_target("${TARGET}" ALL)
  endif()


  ### Specify root folder for staging python modules for this target
  if(arg_STAGING_DIR)
    cmake_path(APPEND "${CMAKE_CURRENT_BINARY_DIR}" "${arg_STAGING_DIR}"
      OUTPUT_VARIABLE staging_root)
  else()
     set(staging_root "${CMAKE_BINARY_DIR}/python-staging")
     # set(staging_root "${CMAKE_CURRENT_BINARY_DIR}/staging")
  endif()

  ### Construct namespace for Python modules
  get_namespace_dir(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${arg_NAMESPACE_COMPONENT}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
    OUTPUT_VARIABLE namespace_dir)

  cmake_path(APPEND staging_root "${namespace_dir}" OUTPUT_VARIABLE staging_dir)
  file(MAKE_DIRECTORY "${staging_dir}")

  ### Construct a list of staging roots for this target and its dependencies.
  cascade_inherited_property(
    TARGET "${TARGET}"
    PROPERTY python_paths
    OUTPUT_VARIABLE python_paths
    INITIAL_VALUE "${staging_root}"
    DEPENDENCIES "${arg_PYTHON_DEPS}"
    REMOVE_DUPLICATES
  )

  ### Construct a list of Python output folders for each ProtoBuf dependency.
  cascade_inherited_property(
    TARGET "${TARGET}"
    PROPERTY python_paths
    OUTPUT_VARIABLE proto_paths
    DEPENDENCIES "${arg_PROTO_DEPS}"
    REMOVE_DUPLICATES
  )

  ### Also add staging folder from indirect protobuf dependencies
  cascade_inherited_property(
    TARGET "${TARGET}"
    PROPERTY proto_paths
    OUTPUT_VARIABLE proto_paths
    INITIAL_VALUE "${proto_paths}"
    DEPENDENCIES "${arg_PYTHON_DEPS}"
    REMOVE_DUPLICATES
  )

  ### Copy programs, files and directories to the staging subfolder
  foreach(_file ${arg_PROGRAMS})
    file(COPY "${_file}"
      DESTINATION "${staging_dir}"
      FILE_PERMISSIONS
      OWNER_READ OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE)
  endforeach()

  foreach(_file ${arg_FILES})
    file(COPY "${_file}"
      DESTINATION "${staging_dir}")
  endforeach()

  foreach(_dir ${arg_DIRECTORIES})
    file(COPY "${_dir}"
      DESTINATION "${staging_dir}"
      PATTERN __pycache__ EXCLUDE)
  endforeach()


  ### Install these modules locally or via CPack
  if (arg_INSTALL OR INSTALL_PYTHON_MODULES)
    if (arg_INSTALL_DIR)
      set(_install_dir "${arg_INSTALL_DIR}")
    else()
      set(_install_dir "${PYTHON_INSTALL_DIR}")
    endif()

    cmake_path(APPEND _install_dir "${namespace_dir}" OUTPUT_VARIABLE _destination)

    if (arg_PROGRAMS)
      install(
        PROGRAMS ${arg_PROGRAMS}
        DESTINATION "${_destination}"
        COMPONENT "${arg_INSTALL_COMPONENT}")
    endif()

    if (arg_FILES)
      install(
        FILES ${arg_FILES}
        DESTINATION "${_destination}"
        COMPONENT "${arg_INSTALL_COMPONENT}")
    endif()

    if (arg_DIRECTORIES)
      install(
        DIRECTORY ${arg_DIRECTORIES}
        DESTINATION "${_destination}"
        COMPONENT "${arg_INSTALL_COMPONENT}")
    endif()

    # install(DIRECTORY "${staging_root}/"
    #   DESTINATION "${_install_dir}"
    #   COMPONENT "${arg_INSTALL_COMPONENT}")
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
      set(namespace "${PYTHON_NAMESPACE}")
    endif()

    if(arg_NAMESPACE_COMPONENT)
      string(JOIN "." namespace "${namespace}" "${arg_NAMESPACE_COMPONENT}")
    endif()
  endif()

  set("${arg_OUTPUT_VARIABLE}" "${namespace}" PARENT_SCOPE)
endfunction()


#===============================================================================
## @fn CASCADE_INHERITED_PROPERTY
## @brief
##    Build a list of values stored as a property on the specified target,
##    and subsequently/recursively in its downstream dependents

function(CASCADE_INHERITED_PROPERTY)
  set(_options REMOVE_DUPLICATES)
  set(_singleargs PROPERTY TARGET INITIAL_VALUE OUTPUT_VARIABLE)
  set(_multiargs DEPENDENCIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(_values "${arg_INITIAL_VALUE}")
  foreach(_dep ${arg_DEPENDENCIES})
    get_target_property(_inherited_values "${_dep}" ${arg_PROPERTY})

    if(_inherited_values)
      list(APPEND _values "${_inherited_values}")
    endif()
  endforeach()

  if(arg_REMOVE_DUPLICATES)
    list(REMOVE_DUPLICATES _values)
  endif()

  if (arg_TARGET)
    set_target_properties("${arg_TARGET}"
      PROPERTIES "${arg_PROPERTY}" "${_values}")
  endif()

  if (arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}" "${_values}" PARENT_SCOPE)
  endif()

endfunction()
