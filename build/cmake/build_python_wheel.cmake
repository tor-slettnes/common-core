## -*- cmake -*-
#===============================================================================
## @file cc_add_python_wheel.cmake
## @brief Build a Python redistributable package (`.whl`)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_WHEEL_DIR "share/python-wheels"
  CACHE STRING "Installation directory for Python  wheels")

cmake_path(APPEND PYTHON_OUT_DIR "wheels"
  OUTPUT_VARIABLE PYTHON_WHEEL_STAGING_ROOT)

set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_WHEEL_STAGING_ROOT}
)

#===============================================================================
## @fn cc_add_python_wheel
## @brief
##  Build a Python binary distribution package ("wheel", or `.whl` file)

function(cc_add_python_wheel TARGET)
  set(_options)
  set(_singleargs
    PACKAGE_NAME PACKAGE_NAME_PREFIX DESCRIPTION VERSION CONTACT URL
    VENV PYTHON_INTERPRETER PYPROJECT_TEMPLATE INSTALL_COMPONENT INSTALL_DIR)
  set(_multiargs
    PROGRAMS BUILD_DEPS PYTHON_DEPS DATA_DEPS PACKAGE_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})


  ### Do this only if the option `BUILD_PYTHON_WHEELS` is enabled
  if(NOT BUILD_PYTHON_WHEELS)
    message(DEBUG "Skipping Python wheel ${TARGET}, as BUILD_PYTHON_WHEELS is disabled")
    return()
  endif()

  cc_find_python(
    ACTION "cc_add_python_wheel(${TARGET})"
    PYTHON_INTERPRETER "${arg_PYTHON_INTERPRETER}"
    VENV "${arg_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  #-----------------------------------------------------------------------------
  ## Assign some variables based on provided inputs and project defaults

  cc_get_value_or_default(
    pyproject_template
    arg_PYPROJECT_TEMPLATE
    "${PYTHON_TEMPLATE_DIR}/pyproject.toml.in")

  cc_get_value_or_default(
    PACKAGE_NAME_PREFIX
    arg_PACKAGE_NAME_PREFIX
    "${PACKAGE_NAME_PREFIX}")

  cc_get_value_or_default(
    PACKAGE_NAME
    arg_PACKAGE_NAME
    "${PACKAGE_NAME_PREFIX}_${TARGET}")

  cc_get_value_or_default(
    DESCRIPTION
    arg_DESCRIPTION
    "${PROJECT_DESCRIPTION}")

  cc_get_value_or_default(
    VERSION
    arg_VERSION
    "${PROJECT_VERSION}")

  if (arg_PACKAGE_DEPS)
    set(wheel_dependencies "${arg_PACKAGE_DEPS}")
  else()
    file(STRINGS "${PYTHON_PIP_REQUIREMENTS_FILE}" wheel_dependencies)
  endif()

  cc_join_quoted(wheel_dependencies
    OUTPUT_VARIABLE DEPENDENCIES)

  cc_get_value_or_default(
    CONTACT
    arg_CONTACT
    "${PACKAGE_CONTACT}")

  cc_get_value_or_default(
    URL
    arg_URL
    "${PROJECT_HOMEPAGE_URL}")

  if(CPACK_PACKAGE_CONTACT)
    string(REGEX MATCH "^([^<]+) ?(<(.*)>)?" _match "${CPACK_PACKAGE_CONTACT}")
    set(AUTHOR_NAME "${CMAKE_MATCH_1}")
    set(AUTHOR_EMAIL "${CMAKE_MATCH_3}")
  endif()

  cc_get_wheel_name(
    PACKAGE_NAME "${PACKAGE_NAME}"
    VERSION "${VERSION}"
    OUTPUT_VARIABLE wheel_name)


  ### Define output directories for `pyproject.toml` and the resulting wheel
  set(gen_dir "${PYTHON_WHEEL_STAGING_ROOT}/${TARGET}")
  set(wheel_dir "${PYTHON_WHEEL_STAGING_ROOT}")
  set(wheel_path "${wheel_dir}/${wheel_name}")

  file(REMOVE_RECURSE "${gen_dir}")

  ### Create TARGET with dependencies
  add_custom_target("${TARGET}" ALL
    DEPENDS "${wheel_path}"
  )

  if(arg_PYTHON_DEPS OR arg_DATA_DEPS)
    add_dependencies("${TARGET}"
      ${arg_PYTHON_DEPS}
      ${arg_DATA_DEPS})
  endif()


  #-----------------------------------------------------------------------------
  ## Collect Python staging directories from targets listed in `PYTHON_DEPS`,
  ## and generate corresponding `"SOURCE_DIR" = "/"` statements for the
  ## `[...force-include]` section in `pyproject.toml`.

  set(package_map)

  cc_get_target_properties_recursively(
    PROPERTIES staging_dir
    TARGETS ${arg_PYTHON_DEPS}
    PREFIX "\""
    SUFFIX "\" = \"/\""
    OUTPUT_VARIABLE dep_staging_dirs
    REMOVE_DUPLICATES
    REQUIRED)

  list(APPEND package_map ${dep_staging_dirs})

  ## Likewise, collect staged source+destination folders listed in `DATA_DEPS`:
  cc_get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_DATA_DEPS}
    PREFIX "\""
    SEPARATOR "\" = \""
    SUFFIX "/\""
    OUTPUT_VARIABLE extra_data
    ALL_OR_NOTHING
    REMOVE_DUPLICATES)

  list(APPEND package_map ${extra_data})

  ### Create a multi-line string in PACKAGE_MAP holding these directory mappings.
  list(JOIN package_map "\n" PACKAGE_MAP)

  #-----------------------------------------------------------------------------
  ### Now create `pyproject.toml` based on the above contents
  #file(REMOVE_RECURSE "${gen_dir}")
  file(MAKE_DIRECTORY "${wheel_dir}")

  configure_file(
    "${pyproject_template}"
    "${gen_dir}/pyproject.toml")

  ### Define command to build wheel
  cc_get_target_property_recursively(
    PROPERTY SOURCES
    TARGETS ${TARGET}
    OUTPUT_VARIABLE sources
    REMOVE_DUPLICATES)

  add_custom_command(
    OUTPUT "${wheel_path}"
    DEPENDS ${arg_BUILD_DEPS} ${arg_PYTHON_DEPS} ${arg_DATA_DEPS} ${sources}
    BYPRODUCTS "${gen_dir}"
    COMMAND hatchling build -d "${wheel_dir}" > /dev/null
    # COMMAND ${python}
    # ARGS -m build --wheel --outdir "${wheel_dir}" "."
    COMMENT "Building Python Wheel: ${wheel_name}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    WORKING_DIRECTORY "${gen_dir}"
  )

  ### Install/package resulting executable
  if(arg_INSTALL_COMPONENT)
    cc_get_value_or_default(
      install_dir
      arg_INSTALL_DIR
      "${PYTHON_WHEEL_DIR}")

    install(
      FILES "${wheel_path}"
      DESTINATION "${install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()
endfunction()


#===============================================================================
## @fn cc_get_wheel_name
## @brief
##  Guesstimate name of `.whl` package file

function(cc_get_wheel_name)
  set(_options)
  set(_singleargs PACKAGE_NAME VERSION OUTPUT_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(REPLACE "-" "_" stem "${arg_PACKAGE_NAME}")
  if(arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}"
      "${stem}-${arg_VERSION}-py3-none-any.whl"
      PARENT_SCOPE)
  endif()
endfunction()


