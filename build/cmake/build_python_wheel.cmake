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

cmake_path(SET PYTHON_WHEEL_OUT_DIR "${PYTHON_OUT_DIR}/wheels")
cmake_path(SET PYTHON_WHEEL_BUILD_DIR "${PYTHON_WHEEL_OUT_DIR}/build")

set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_WHEEL_OUT_DIR}
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
    VERSION             # Explicit version, if not "${PROJECT_VERSION}.${BUILD_NUMBER}"
    CONTACT             # Package contact (Real Name <email@addr.ess>)
    URL                 # Package publisher URL
    BUILD_VENV          # Use a Python VENV to generate package (optional)
    LOCAL_VENV_TARGET   # Install wheel into a local Python VENV by build target
    LOCAL_VENV          # Install wheel into a local Python VENV by path
    INSTALL_COMPONENT   # CPack component into which this wheel is added
    INSTALL_DIR         # Override default installation folder
    INSTALL_VENV        # Add Debian post-install hook to install wheel into a `venv`
    INSTALL_SYMLINKS    # Create symbolic links to VENV scripts here (e.g., `bin` or `sbin`)
  )
  set(_multiargs
    BUILD_DEPS          # CMake targets that must be built before this
    PYTHON_DEPS         # Python target dependencies to include in wheel
    WHEEL_DEPS          # Other wheels that must also be built & installed
    DATA_DEPS           # Other target dependencies to include, e.g. settings
    REQUIREMENTS        # Direct Python package dependencies
    REQUIREMENTS_FILES  # Custom Python package requirements file (for `pip install`)
    REQUIREMENTS_DEPS   # Python package/distribution cache dependencies
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
    "${CMAKE_PROJECT_VERSION}.${BUILD_NUMBER}")

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

  #-----------------------------------------------------------------------------
  # Determine and assign Python package dependencies to `DEPENDENCIES`, which is
  # expanded in the input template `../python/pyproject.toml.in`.

  cc_get_python_requirements_list(
    REQUIREMENTS "${arg_REQUIREMENTS}"
    REQUIREMENTS_FILES "${arg_REQUIREMENTS_FILES}"
    REQUIREMENTS_DEPS "${arg_REQUIREMENTS_DEPS}"
    OUTPUT_VARIABLE requirements_list
  )

  foreach(wheel_dep ${arg_WHEEL_DEPS})
    get_target_property(wheel_name ${wheel_dep} "wheel_name")
    if(wheel_name)
      get_target_property(wheel_version ${wheel_dep} wheel_version)
      list(APPEND requirements_list ${wheel_name}==${wheel_version})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES requirements_list)
  cc_join_quoted(requirements_list
    OUTPUT_VARIABLE DEPENDENCIES)


  #--------------------------------------------------------------------------
  # Define output directories for `pyproject.toml` and the resulting wheel

  set(gen_dir "${PYTHON_WHEEL_BUILD_DIR}/${TARGET}")
  file(REMOVE_RECURSE "${gen_dir}")

  set(wheel_dir "${PYTHON_WHEEL_OUT_DIR}")
  set(wheel_path "${wheel_dir}/${wheel_file}")


  #-----------------------------------------------------------------------------
  # Collect staging directories from targets listed in `PYTHON_DEPS` and
  # `DATA_DEPS`, and their dependencies. However, some of these build targets
  # may also be indirect dependencies of other wheels listed in `WHEEL_DEPS`,
  # and would those be installed multiple times on the target.  Thus, we first
  # need to subtract those from our dependency list.

  cc_get_target_property_recursively(
    PROPERTY "NAME"
    TARGETS ${arg_PYTHON_DEPS} ${arg_DATA_DEPS}
    OUTPUT_VARIABLE deps)

  cc_get_target_property_recursively(
    PROPERTY "NAME"
    TARGETS ${arg_WHEEL_DEPS}
    OUTPUT_VARIABLE indirect_deps)

  list(REMOVE_ITEM deps ${indirect_deps})


  #-----------------------------------------------------------------------------
  # Collect staging directories from the resulting direct dependencies, and
  # generate corresponding `"SOURCE_DIR" = "TARGET_DIR/"` statements for the
  # `[...force-include]` section in `pyproject.toml`.

  cc_get_target_properties_recursively(
    PROPERTIES "staging_dir" "data_dir"
    TARGETS ${deps}
    PREFIX "\""
    SEPARATOR "\" = \""
    SUFFIX "/\""
    OUTPUT_VARIABLE package_map
    REMOVE_DUPLICATES)


  # Create a multi-line string in PACKAGE_MAP holding these directory mappings.
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

  file(MAKE_DIRECTORY "${wheel_dir}")

  message(VERBOSE
    "Creating Python wheel: "
    "PACKAGE_NAME=${PACKAGE_NAME}, "
    "VERSION=${VERSION}, "
    "DEPENDENCIES=${DEPENDENCIES}, "
    "AUTHOR_NAME=${AUTHOR_NAME}, "
    "AUTHOR_EMAIL=${AUTHOR_EMAIL}, "
    "CONTENTS: ${PACKAGE_MAP}"
  )

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
    ${arg_DATA_DEPS}
  )

  ## Add this target as a dependency for `python_wheels`
  add_dependencies(${PYTHON_WHEELS_TARGET} ${TARGET})

  ## Set target properties used by downstream dependents
  set_target_properties(${TARGET} PROPERTIES
    wheel_name "${PACKAGE_NAME}"
    wheel_version "${VERSION}"
    wheel_file "${wheel_file}"
    wheel_path "${wheel_path}"
  )

  #-----------------------------------------------------------------------------
  # Create Python wheel

  cc_get_target_property_recursively(
    PROPERTY "SOURCES"
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
      "${PYTHON_WHEELS_INSTALL_DIR}/${TARGET}")

    set_target_properties(${TARGET} PROPERTIES
      python_distributions_install_dir "${wheels_install_dir}"
    )

    install(
      FILES "${wheel_path}"
      DESTINATION "${wheels_install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )


    if(arg_INSTALL_VENV)
      cc_get_target_property_recursively(
        PROPERTY "python_distributions_install_dir"
        TARGETS ${arg_WHEEL_DEPS} ${arg_REQUIREMENTS_DEPS}
        REMOVE_DUPLICATES
        OUTPUT_VARIABLE wheel_install_dirs)

      cc_add_python_wheel_install_hook(
        VENV_PATH "${arg_INSTALL_VENV}"
        WHEEL_NAME "${PACKAGE_NAME}"
        WHEEL_INSTALL_DIRS "${wheel_install_dirs}"
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
    set(venv_target "${arg_LOCAL_VENV_TARGET}")

  elseif(arg_LOCAL_VENV)
    set(venv_target "${TARGET}-venv")
    cc_add_python_venv(${venv_target}
      VENV_PATH "${arg_LOCAL_VENV}")
  endif()

  if(venv_target)
    get_target_property(venv_path   ${venv_target} venv_path)
    get_target_property(venv_python ${venv_target} venv_python)

    cmake_path(APPEND venv_path ".build-stamps" OUTPUT_VARIABLE stamp_dir)
    cmake_path(APPEND stamp_dir "${TARGET}"     OUTPUT_VARIABLE target_stamp)

    cmake_path(RELATIVE_PATH venv_path
      BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE venv_rel_path)

    cc_get_target_properties_recursively(
      TARGETS ${arg_WHEEL_DEPS} ${arg_REQUIREMENTS_DEPS}
      PROPERTIES python_distributions_cache_dir
      PREFIX "--find-links="
      REMOVE_DUPLICATES
      OUTPUT_VARIABLE distcache_args)

    cc_get_target_properties_recursively(
      TARGETS ${arg_WHEEL_DEPS}
      PROPERTIES wheel_path
      INITIAL_VALUE "${wheel_path}"
      OUTPUT_VARIABLE wheel_paths)

    #---------------------------------------------------------------------------
    # Add `-install` target to install wheel into virtualenv.

    add_custom_target(${TARGET}-install
      DEPENDS ${TARGET} ${TARGET}-uninstall ${venv_target} ${arg_REQUIREMENTS_DEPS}
      COMMENT "Installing wheel '${PACKAGE_NAME}' into '${venv_rel_path}'"
      COMMAND ${venv_python} -m pip install ${PIP_QUIET} ${distcache_args} "${wheel_paths}"
      VERBATIM
      COMMAND_EXPAND_LISTS
    )

    add_dependencies(${TARGET}-install
      ${TARGET}
      ${venv_target}
      ${arg_REQUIREMENTS_DEPS}
    )

    #---------------------------------------------------------------------------
    # Add `-uninstall` target to remove wheel/prepare for reinstall.

    add_custom_target(${TARGET}-uninstall
      COMMAND "${venv_python}" -m pip uninstall ${PIP_QUIET} ${PIP_QUIET} --yes "${PACKAGE_NAME}"
      VERBATIM
    )

    add_dependencies(${TARGET}-uninstall
      ${venv_target})

    #---------------------------------------------------------------------------
    # Ensure dependencies get installed along with wheel.
    # Also ensure wheel gets uninstalled along with any dependency.

    foreach(wheel_dep ${arg_WHEEL_DEPS})
      if(TARGET ${wheel_dep}-install)
        add_dependencies(${TARGET}-install ${wheel_dep}-install)
        add_dependencies(${wheel_dep}-uninstall ${TARGET}-uninstall)
      endif()
    endforeach()

    # add_dependencies(${TARGET}-install
    #   ${TARGET}-uninstall
    #   ${arg_REQUIREMENTS_DEPS}
    # )
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



#-------------------------------------------------------------------------------
## @fn cc_add_python_wheel_install_hook
## @brief
##   Add post-install/pre-removal hook to add/remove Python virtual environment

function(cc_add_python_wheel_install_hook)
  set(_options
    ALLOW_DOWNLOADS     # Allow requirements to be downloaded on install
    VERBOSE             # Install wheels without `--quiet` option
  )
  set(_singleargs
    WHEEL_NAME          # Base name of package/distribution to install
    WHEEL_VERSION       # Version of package/distribution to install
    VENV_PATH           # Target `virtualenv` folder.
    COMPONENT           # CPack component to which hook is added
    SYMLINKS_TARGET_DIR # Create symlinks in this folder (optional)
    POSTINST_TEMPLATE   # Use custom template for `postinst` script
    PRERM_TEMPLATE      # Use custom template for `prerm` script
  )
  set(_multiargs
    WHEEL_INSTALL_DIRS  # Directories in which to find required Python distributions
    SYMLINKS            # Executables for which to create symlinks in /usr/bin
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_cpack_get_debian_grouping(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    OUTPUT_VARIABLE package_name)

  cmake_path(APPEND DEBIAN_CONTROL_STAGING_DIR ${package_name}
    OUTPUT_VARIABLE staging_dir)

  #-----------------------------------------------------------------------------
  # Assign variable values for control scripts.
  # See `../debian/wheels-postinst.in` and `../debian/wheels-prerm.in`.

  # `WHEEL_NAME` is the base name of the wheel without version or suffix
  set(WHEEL_NAME "${arg_WHEEL_NAME}")

  # `WHEEL_VERSION` is the wheel version
  set(WHEEL_VERSION "${arg_WHEEL_VERSION}")

  # `WHEEL_SPEC` is the combined name and version if specified, otherwise just the name.
  cc_get_ternary(WHEEL_SPEC
    arg_WHEEL_VERSION
    "${arg_WHEEL_NAME}==${arg_WHEEL_VERSION}"
    "${arg_WHEEL_NAME}")

  # `ALLOW_DOWNLOADS` allows downloading Python required distributions when creating `venv`
  cc_get_ternary(ALLOW_DOWNLOADS
    arg_ALLOW_DOWNLOADS
    true
    false)

  cc_get_ternary(VERBOSE
    arg_VERBOSE
    true
    false)

  # `VENV_PATH` is the absolute path name to the environment we are populating
  # (This may be shared with additional wheels or it may be created new).
  cc_get_value_or_default(VENV_PATH
    arg_VENV_PATH
    ${PYTHON_VENV_ROOT}/${WHEEL_NAME})
  cmake_path(ABSOLUTE_PATH VENV_PATH
    BASE_DIRECTORY "${PYTHON_VENV_ROOT}")

  # `WHEEL_INSTALL_DIRS` is where to find the wheel and its requirements
  unset(install_dirs)
  foreach(install_dir ${arg_WHEEL_INSTALL_DIRS})
    cmake_path(ABSOLUTE_PATH install_dir
      BASE_DIRECTORY ${INSTALL_ROOT})
    list(APPEND install_dirs "${install_dir}")
  endforeach()

  cc_join_quoted(install_dirs
    GLUE " "
    OUTPUT_VARIABLE WHEEL_INSTALL_DIRS)

  # `SYMLINKS` is a list of executable scripts for which we want to create
  # symbolic links, e.g. in `/usr/bin` or `/usr/sbin`.
  list(TRANSFORM arg_SYMLINKS
    PREPEND "${VENV_PATH}/bin/"
    OUTPUT_VARIABLE executables)

  cc_join_quoted(executables
    GLUE " "
    OUTPUT_VARIABLE EXECUTABLES)

  # `SYMLINKS_TARGET_DIR` is where we want to install symlinks
  if(arg_SYMLINKS_TARGET_DIR)
    cmake_path(ABSOLUTE_PATH arg_SYMLINKS_TARGET_DIR
      BASE_DIRECTORY "${INSTALL_ROOT}"
      OUTPUT_VARIABLE SYMLINKS_TARGET_DIR)
  endif()


  #-----------------------------------------------------------------------------
  # Stage and add Debian postinst/prerm scripts

  cc_get_argument_or_default(postinst_template
    arg_POSTINST_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/wheel-postinst.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  cc_get_argument_or_default(prerm_template
    arg_PRERM_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/wheel-prerm.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  set(script_name "${WHEEL_NAME}-venv")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "postinst"
    TEMPLATE "${postinst_template}"
    OUTPUT_FILE "20-${script_name}")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "prerm"
    TEMPLATE "${prerm_template}"
    OUTPUT_FILE "80-${script_name}")
endfunction()
