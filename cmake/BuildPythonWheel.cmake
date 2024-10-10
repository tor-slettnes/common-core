## -*- cmake -*-
#===============================================================================
## @file BuildPythonWheel.cmake
## @brief Build a Python redistributable package (`.whl`)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(PYTHON_WHEEL_DIR "share/python-wheels"
  CACHE STRING "Installation directory for Python  wheels")

#===============================================================================
## @fn BuildPythonWheel
## @brief
##  Build a Python binary distribution package ("wheel", or `.whl` file)

function(BuildPythonWheel TARGET)
  set(_options)

  set(_singleargs
    PACKAGE DESCRIPTION VERSION CONTACT URL
    VENV PYTHON_INTERPRETER PYPROJECT_TEMPLATE INSTALL_COMPONENT INSTALL_DIR)

  set(_multiargs
    PROGRAMS BUILD_DEPS PYTHON_DEPS SETTINGS_DEPS PACKAGE_DEPS)

  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Do this only if the option `BUILD_PYTHON_WHEELS` is enabled
  if(NOT BUILD_PYTHON_WHEELS)
    return()
  endif()

  ### Copy sources from the specified target dependencies into a consolidated
  ### staging area.
  set(wheel_dir "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}")
  set(wheel_base "${wheel_dir}/${TARGET}")
  set(wheel_path "${wheel_base}")

  ### Determine Python interpreter from PYTHON_INTERPRETER, VENV, or host native
  if(arg_PYTHON_INTERPRETER)
    set(python "${arg_PYTHON_INTERPRETER}")
  elseif(arg_VENV)
    set(python "${arg_VENV}/bin/python")
  else()
    find_package(Python3
      COMPONENTS Interpreter
    )

    if(Python3_Interpreter_FOUND)
      set(python "${Python3_EXECUTABLE}")
    else()
      message(FATAL_ERRROR "BuildPythonWheel() requires a Python interpreter")
    endif()
  endif()


  add_custom_target("${TARGET}" ALL DEPENDS "${wheel_path}")
  add_dependencies("${TARGET}" ${arg_BUILD_DEPS} ${arg_PYTHON_DEPS})

  ### Collect staged python modules from targets listed in `PYTHON_DEPS`,
  ### and generate a corresponding multi-line string in `PYTHON_INCLUDE`
  ### suitable for the `[...force-include]` section of `pyproject.toml`
  ### (using the "hatchling" build back-end).

  create_include_map(
    DEPENDENCIES ${arg_PYTHON_DEPS}
    PROPERTY staging_dir
    TARGET_DIR "/"
    OUTPUT_VARIABLE PYTHON_INCLUDE
    REQUIRED)

  ### Likewise, collect staged settings folders from targets listed in
  ### `SETTTINS_DEPS`, producing a suitable `SETTINGS_INCLUDE` string.

  create_include_map(
    DEPENDENCIES ${arg_SETTINGS_DEPS}
    PROPERTY staging_dir
    TARGET_DIR "settings/"
    OUTPUT_VARIABLE SETTINGS_INCLUDE)


  ### Now create `pyproject.toml` based on the above contents
  get_value_or_default(
    pyproject_template
    arg_PYPROJECT_TEMPLATE
    "${PYTHON_TEMPLATE_DIR}/pyproject.toml.in")

  get_value_or_default(
    PACKAGE
    arg_PACKAGE
    "${TARGET}")

  get_value_or_default(
    DESCRIPTION
    arg_DESCRIPTION
    "${PROJECT_DESCRIPTION}")

  get_value_or_default(
    VERSION
    arg_VERSION
    "${PROJECT_VERSION}")

  get_value_or_default(
    DEPENDENCIES
    arg_PACKAGE_DEPS
    "${PYTHON_DEPENDENCIES}")

  get_value_or_default(
    CONTACT
    arg_CONTACT
    "${PACKAGE_CONTACT}")

  get_value_or_default(
    URL
    arg_URL
    "${PROJECT_HOMEPAGE_URL}")

  if(CONTACT)
    string(REGEX MATCH "^([^<]+) ?(<(.*)>)?" _match "${CONTACT}")
    set(AUTHOR_NAME "${CMAKE_MATCH_1}")
    set(AUTHOR_EMAIL "${CMAKE_MATCH_3}")
  endif()

  configure_file(
    "${pyproject_template}"
    "pyproject.toml")

  ### Define command to build wheel
  file(MAKE_DIRECTORY "${wheel_dir}")
  add_custom_command(
    OUTPUT "${wheel_path}"
    COMMAND ${python}
    ARGS -m build --wheel --outdir "${wheel_dir}" "."
    COMMENT "Building PyIntaller executable: ${program}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  )


  ### Install/package resulting executable
  if(arg_INSTALL_COMPONENT)
    get_value_or_default(
      install_dir
      arg_INSTALL_DIR
      "${PYTHON_WHEEL_DIR}")

    install(
      DIRECTORY "${wheel_dir}/"
      DESTINATION "${install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()
endfunction()


#===============================================================================
## @fn create_include_map
## @brief Generate include specifications for the Hatchling build back-end
##
## Collect staged python modules from targets listed in `PYTHON_DEPS`,
## and generate a corresponding multi-line string in `PYTHON_INCLUDE`
## suitable for the `[...force-include]` section of `pyproject.toml`
## (using the "hatchling" build back-end).

function(create_include_map)
  set(_options REQUIRED)
  set(_singleargs PROPERTY TARGET_DIR OUTPUT_VARIABLE)
  set(_multiargs DEPENDENCIES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  get_optional_keyword(REQUIRED "${arg_REQUIRED}")

  get_target_properties_recursively(
    PROPERTIES "${arg_PROPERTY}"
    TARGETS "${arg_DEPENDENCIES}"
    OUTPUT_VARIABLE include_dirs
    REMOVE_DUPLICATES
    ${REQUIRED})

  list(TRANSFORM include_dirs
    REPLACE "^(.+)$" "\"\\1\" = \"${arg_TARGET_DIR}\""
    OUTPUT_VARIABLE include_map)

  list(JOIN include_map "\n" include_spec)
  set("${arg_OUTPUT_VARIABLE}" "${include_spec}" PARENT_SCOPE)
endfunction()
