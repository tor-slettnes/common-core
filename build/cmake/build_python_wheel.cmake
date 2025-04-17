## -*- cmake -*-
#===============================================================================
## @file build_python_wheel.cmake
## @brief Build a Python redistributable package (`.whl`)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

#-------------------------------------------------------------------------------
## Add global `python_wheels` target, into which wheels will be dependencies

set(PYTHON_WHEELS_TARGET "python_wheels")
add_custom_target(${PYTHON_WHEELS_TARGET})

cmake_path(APPEND PYTHON_OUT_DIR "wheels"
  OUTPUT_VARIABLE PYTHON_WHEEL_STAGING_ROOT)

set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_WHEEL_STAGING_ROOT}
)

#-------------------------------------------------------------------------------
## @fn cc_add_python_wheel
## @brief
##  Build a Python binary distribution package ("wheel", or `.whl` file)

function(cc_add_python_wheel TARGET)
  set(_options
    ALL                 # Add wheel to the default default build target
  )
  set(_singleargs
    PACKAGE_NAME_PREFIX # Override default prefix
    PACKAGE_NAME_MAIN   # Set package name (following prefix), otherwise TARGET
    PACKAGE_NAME        # Override entire package name, otherwise PREFIX_MAIN
    DESCRIPTION         # One-line description field
    PYPROJECT_TEMPLATE  # `pyproject.toml` template file
    REQUIREMENTS_FILE   # Custom Python package requirements file (for `pip install`)
    VERSION             # Explicit version, if not "${PROJECT_VERSION}.${BUILD_NUMBER}"
    CONTACT             # Package contact (Real Name <email@addr.ess>)
    URL                 # Package publisher URL
    BUILD_VENV          # Use a Python VENV to generate package (optional)
    LOCAL_VENV_TARGET   # Install wheel into a local Python VENV by build target
    INSTALL_COMPONENT   # CPack component into which this wheel is added
    INSTALL_DIR         # Override default installation folder
    INSTALL_VENV        # Add Debian post-install hook to install wheel into a `venv`
    INSTALL_SYMLINKS    # Create symbolic links to VENV scripts here (e.g., `/usr/bin`)
  )
  set(_multiargs
    BUILD_DEPS          # CMake targets that must be built before this
    PYTHON_DEPS         # Python target dependencies to include in wheel
    WHEEL_DEPS          # Other wheels that must also be built & installed
    DATA_DEPS           # Other target dependencies to include, e.g. settings
    DISTCACHE_DEPS      # Python package/distribution cache dependencies
    REQUIREMENTS        # Direct Python package dependencies
    SCRIPTS             # Executable script(s). Format: `SCRIPT:MODULE:METHOD`.
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_find_python(
    ACTION "cc_add_python_wheel(${TARGET})"
    VENV "${arg_BUILD_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  #-----------------------------------------------------------------------------
  # Assign some variables based on provided inputs and project defaults

  cc_get_value_or_default(
    pyproject_template
    arg_PYPROJECT_TEMPLATE
    "${PYTHON_TEMPLATE_DIR}/pyproject.toml.in")

  cc_get_value_or_default(
    package_name_prefix
    arg_PACKAGE_NAME_PREFIX
    "${PYTHON_WHEEL_NAME_PREFIX}")

  cc_get_value_or_default(
    package_name_main
    arg_PACKAGE_NAME_MAIN
    "${TARGET}")

  cc_get_value_or_default(
    PACKAGE_NAME
    arg_PACKAGE_NAME
    "${package_name_prefix}${package_name_main}")

  cc_get_value_or_default(
    DESCRIPTION
    arg_DESCRIPTION
    "${PROJECT_DESCRIPTION}")

  cc_get_value_or_default(
    VERSION
    arg_VERSION
    "${PROJECT_VERSION}.${BUILD_NUMBER}")

  cc_get_value_or_default(
    URL
    arg_URL
    "${PROJECT_HOMEPAGE_URL}")

  cc_get_value_or_default(
    CONTACT
    arg_CONTACT
    "${CPACK_PACKAGE_CONTACT}")

  if(CONTACT)
    string(REGEX MATCH "^([^<]+) ?(<(.*)>)?" _match "${CONTACT}")
    set(AUTHOR_NAME "${CMAKE_MATCH_1}")
    set(AUTHOR_EMAIL "${CMAKE_MATCH_3}")
  endif()

  cc_get_wheel_file(
    PACKAGE_NAME "${PACKAGE_NAME}"
    VERSION "${VERSION}"
    OUTPUT_VARIABLE wheel_file)

  #--------------------------------------------------------------------------
  # Determine and assign Python package dependencies to DEPENDENCIES

  if(arg_REQUIREMENTS_FILE)
    set(requirements_file "${arg_REQUIREMENTS_FILE}")
    file(STRINGS "${requirements_file}" requirements_list)
  endif()

  list(APPEND requirements_list ${arg_REQUIREMENTS})

  cc_get_target_property_recursively(
    PROPERTY python_distributions
    TARGETS ${arg_DISTCACHE_DEPS}
    INITIAL_VALUE "${requirements_list}"
    OUTPUT_VARIABLE requirements_list
  )

  cc_join_quoted(requirements_list
    OUTPUT_VARIABLE DEPENDENCIES)


  #--------------------------------------------------------------------------
  # Define output directories for `pyproject.toml` and the resulting wheel

  set(gen_dir "${PYTHON_WHEEL_STAGING_ROOT}/${TARGET}")
  set(wheel_dir "${PYTHON_WHEEL_STAGING_ROOT}")
  set(wheel_path "${wheel_dir}/${wheel_file}")

  file(REMOVE_RECURSE "${gen_dir}")

  #-----------------------------------------------------------------------------
  # Collect staging directories from targets listed in `PYTHON_DEPS` and
  # `DATA_DEPS`, and generate corresponding `"SOURCE_DIR" = "TARGET_DIR/"`
  # statements for the `[...force-include]` section in `pyproject.toml`.

  cc_get_target_properties_recursively(
    PROPERTIES staging_dir data_dir
    TARGETS ${arg_PYTHON_DEPS} ${arg_DATA_DEPS}
    PREFIX "\""
    SEPARATOR "\" = \""
    SUFFIX "/\""
    OUTPUT_VARIABLE package_map
    REMOVE_DUPLICATES)

  ### Create a multi-line string in PACKAGE_MAP holding these directory mappings.
  list(JOIN package_map "\n" PACKAGE_MAP)

  #-----------------------------------------------------------------------------
  # Expand script specifications

  unset(executables)
  unset(script_specs)
  foreach(script_spec ${arg_SCRIPTS})
    if(script_spec MATCHES "^([^:=]+)[:=](.*)$")
      list(APPEND executables ${CMAKE_MATCH_1})
      list(APPEND script_specs "${CMAKE_MATCH_1} = \"${CMAKE_MATCH_2}\"")
    endif()
  endforeach()
  list(JOIN script_specs "\n" SCRIPTS)

  #-----------------------------------------------------------------------------
  # Now create `pyproject.toml` based on the above contents

  #file(REMOVE_RECURSE "${gen_dir}")
  file(MAKE_DIRECTORY "${wheel_dir}")

  configure_file(
    "${pyproject_template}"
    "${gen_dir}/pyproject.toml")



  #-----------------------------------------------------------------------------
  # Create TARGET with dependencies, possibly included in the `ALL` target.

  if(arg_ALL OR WITH_PYTHON_WHEELS)
    set(include_in_all "ALL")
  endif()

  add_custom_target(${TARGET} ${include_in_all}
    DEPENDS ${wheel_path}
  )

  add_dependencies(${TARGET}
    ${arg_BUILD_DEPS}
    ${arg_PYTHON_DEPS}
    ${arg_WHEEL_DEPS}
    ${arg_DATA_DEPS}
    ${arg_DISTCACHE_DEPS}
  )

  ## Add this target as a dependency for `python_wheels`
  add_dependencies(${PYTHON_WHEELS_TARGET} ${TARGET})

  #-----------------------------------------------------------------------------
  # Create Python wheel

  cc_get_target_property_recursively(
    PROPERTY SOURCES
    TARGETS ${TARGET}
    OUTPUT_VARIABLE sources
    REMOVE_DUPLICATES)

  add_custom_command(
    OUTPUT "${wheel_path}"
    DEPENDS ${sources}
    BYPRODUCTS "${gen_dir}"
    COMMAND sh -c "'${python}' -m hatchling build -d '${wheel_dir}' > /dev/null"
    #COMMAND ${python} -m build --wheel --outdir "${wheel_dir}" "."
    COMMENT "Building Python Wheel: ${wheel_file}"
    COMMAND_EXPAND_LISTS
    VERBATIM
    WORKING_DIRECTORY "${gen_dir}"
  )

  #-----------------------------------------------------------------------------
  # Install/package resulting executable

  if(WITH_PYTHON_WHEELS AND arg_INSTALL_COMPONENT)
    cc_get_value_or_default(
      wheels_install_dir
      arg_INSTALL_DIR
      "${PYTHON_WHEELS_INSTALL_DIR}")

    install(
      FILES "${wheel_path}"
      DESTINATION "${wheels_install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )

    if(arg_INSTALL_VENV)
      cc_add_venv_install_hook(
        VENV_PATH "${arg_INSTALL_VENV}"
        WHEEL_NAME "${PACKAGE_NAME}"
        WHEELS_INSTALL_DIR "${wheels_install_dir}"
        WHEEL_VERSION "${VERSION}"
        SYMLINKS ${executables}
        SYMLINKS_TARGET_DIR ${arg_INSTALL_SYMLINKS}
        COMPONENT "${arg_INSTALL_COMPONENT}"
      )
    endif()
  endif()


  #-----------------------------------------------------------------------------
  # Determine if we are deploying the resulting wheel into a local VENV.
  # If so, add targets `${TARGET}-install` and `${TARGET}-uninstall`.

  if(arg_LOCAL_VENV_TARGET)
    get_target_property(venv_path   ${arg_LOCAL_VENV_TARGET} venv_path)
    get_target_property(venv_python ${arg_LOCAL_VENV_TARGET} venv_python)
    get_target_property(stamp_dir   ${arg_LOCAL_VENV_TARGET} stamp_dir)

    cmake_path(RELATIVE_PATH venv_path
      BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE venv_rel_path)

    add_custom_target(${TARGET}-install
      COMMAND ${venv_python} -m pip install ${PIP_QUIET} "${wheel_path}"
      DEPENDS "${TARGET}" "${arg_LOCAL_VENV_TARGET}"
      COMMENT "Installing wheel '${PACKAGE_NAME}' into '${venv_rel_path}'"
      VERBATIM
    )

    add_custom_target(${TARGET}-uninstall
      COMMAND ${venv_python} -m pip uninstall ${PIP_QUIET} ${PIP_QUIET} --yes "${PACKAGE_NAME}"
      DEPENDS "${arg_LOCAL_VENV_TARGET}"
      COMMENT "Uninstalling wheel '${PACKAGE_NAME}' from '${venv_rel_path}'"
      VERBATIM
    )

    foreach(wheel_dep ${arg_WHEEL_DEPS})
      add_dependencies(${TARGET}-install ${wheel_dep}-install)
      add_dependencies(${wheel_dep}-uninstall ${TARGET}-uninstall)
    endforeach()
  endif()

endfunction()



#-------------------------------------------------------------------------------
## @fn cc_get_wheel_file
## @brief
##  Guesstimate name of `.whl` package file (created by `hatchling`)

function(cc_get_wheel_file)
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


