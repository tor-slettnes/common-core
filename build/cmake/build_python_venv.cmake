## -*- cmake -*-
#===============================================================================
## @file build_python_venv.cmake
## @brief Build a Python virtual environment (`virtualenv`)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


if(NOT IS_ABSOLUTE PYTHON_VENV)
  cmake_path(ABSOLUTE_PATH PYTHON_VENV
    BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PYTHON_VENV)
endif()

cmake_path(SET PYTHON_DISTCACHE_DIR "${PYTHON_OUT_DIR}/requirements")
cmake_path(SET PYTHON_VENV_LOCAL_ROOT "${PYTHON_OUT_DIR}/venvs")

set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${PYTHON_VENV_LOCAL_ROOT}
)

if("${CMAKE_MESSAGE_LOG_LEVEL}" MATCHES "^(TRACE|DEBUG|VERBOSE)$")
  unset(PIP_QUIET)
else()
  set(PIP_QUIET "--quiet")
endif()

#-------------------------------------------------------------------------------
# @fn cc_add_python_venv
# @brief
#     Create a Python virtual environment locally.

function(cc_add_python_venv TARGET)
  set(_options ALL)
  set(_singleargs
    LOCAL_PATH          # Virtualenv folder on local (build) machine
    TARGET_PATH         # Add Debian post-install hook to create venv on target machine
    POPULATE_TARGET     # Create dependent target to populate local venv with requirements
    INSTALL_CONDITION   # Boolean variable which if present controls whether to include this target
    INSTALL_COMPONENT   # CPack component to which hook is added
    POSTINST_TEMPLATE   # Use custom template for `postinst` script
    PRERM_TEMPLATE      # Use custom template for `prerm` script
  )
  set(_multiargs
    REQUIREMENTS_FILES  # Files containing package names to install
    REQUIREMENTS        # Add prerequisite target to obtain specific whels
    REQUIREMENTS_DEPS   # Upstream distribution cache targets on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  find_package(Python3
    COMPONENTS Interpreter
    REQUIRED)

  cc_get_value_or_default(local_path arg_LOCAL_PATH "${TARGET}")
  cmake_path(ABSOLUTE_PATH local_path
    BASE_DIRECTORY "${PYTHON_VENV_LOCAL_ROOT}")

  cmake_path(APPEND local_path "bin" "python" OUTPUT_VARIABLE venv_python)

  cc_get_value_or_default(target_path arg_TARGET_PATH "${TARGET}")
  cmake_path(ABSOLUTE_PATH target_path
    BASE_DIRECTORY "${PYTHON_VENV_ROOT}")

  #-----------------------------------------------------------------------------

  if (arg_ALL)
    set(install ON)
  elseif (arg_INSTALL_CONDITION)
    set(install ${${arg_INSTALL_CONDITION}})
  elseif(arg_INSTALL_COMPONENT)
    set(install ON)
  else()
    set(install OFF)
  endif()

  cc_get_optional_keyword(ALL install)
  add_custom_target(${TARGET} ${ALL}
    DEPENDS ${venv_python})

  set_target_properties(${TARGET} PROPERTIES
    local_path "${local_path}"
    venv_python "${venv_python}"
    target_path "${target_path}"
  )

  #-----------------------------------------------------------------------------
  # Create the virtual environment

  cmake_path(RELATIVE_PATH local_path
    BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE venv_rel_path)

  add_custom_command(
    OUTPUT "${venv_python}"
    COMMENT "${TARGET}: Creating Python Virtual Environment: ${venv_rel_path}"
    VERBATIM

    COMMAND "${Python3_EXECUTABLE}" -m virtualenv ${PIP_QUIET} "${local_path}"
  )

  #-----------------------------------------------------------------------------
  # Add dependent target (if given one) to populate the virtual environment with
  # required distributions.

  if(arg_POPULATE_TARGET)
    cc_populate_python_venv("${arg_POPULATE_TARGET}" ${ALL}
      VENV_TARGET "${TARGET}"
      REQUIREMENTS_FILES "${arg_REQUIREMENTS_FILES}"
      REQUIREMENTS "${arg_REQUIREMENTS}"
      REQUIREMENTS_DEPS "${arg_REQUIREMENTS_DEPS}"
    )
    add_dependencies("${arg_POPULATE_TARGET}" "${TARGET}")
  endif()

  #-----------------------------------------------------------------------------
  # Add post-inst hooks to create venv on target

  if(install AND arg_INSTALL_COMPONENT)
    cc_add_python_venv_install_hook(
      VENV_PATH "${target_path}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
      POSTINST_TEMPLATE "${arg_POSTINST_TEMPLATE}"
      PRERM_TEMPLATE "${arg_PRERM_TEMPLATE}"
    )
  endif()

endfunction()


#-------------------------------------------------------------------------------
## @fn cc_get_python_requirements_list
## @brief
##   Get Consolidated list of Python distributions from multiple arguments

function(cc_get_python_requirements_list)
  set(_options
  )
  set(_singleargs
    OUTPUT_VARIABLE     # Caller-scoped variable in which names are stored.
  )
  set(_multiargs
    REQUIREMENTS        # Additional Python distribution (package) names
    REQUIREMENTS_FILES  # File containing required distribution names
    REQUIREMENTS_DEPS   # Upstream distribution cache targets on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(distributions ${arg_REQUIREMENTS})
  foreach(file ${arg_REQUIREMENTS_FILES})
    file(STRINGS "${file}" dists_from_file)
    list(APPEND distributions ${dists_from_file})
  endforeach()

  cc_get_target_property_recursively(
    TARGETS ${arg_REQUIREMENTS_DEPS}
    PROPERTY python_distributions
    INITIAL_VALUE "${distributions}"
    OUTPUT_VARIABLE distributions
  )

  if(arg_OUTPUT_VARIABLE)
    set("${arg_OUTPUT_VARIABLE}" "${distributions}" PARENT_SCOPE)
  endif()
endfunction()


#-------------------------------------------------------------------------------
## @fn cc_add_python_requirements_cache
## @brief
##   Download distributions

function(cc_add_python_requirements_cache TARGET)
  set(_options
    ALL                # Add wheel to the default default build target
  )
  set(_singleargs
    STAGING_DIR        # Override default staging directory
    VENV               # Use Python interpreter from a VENV (optional)
    INSTALL_CONDITION  # Boolean variable which if present controls whether to include this target
    INSTALL_COMPONENT  # Install cache onto target
    INSTALL_DIR        # Override default installation folder
  )
  set(_multiargs
    REQUIREMENTS_FILES # File containing required distribution names
    REQUIREMENTS       # Additional Python distribution (package) names
    REQUIREMENTS_DEPS  # Upstream distribution cache targets on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_find_python(
    ACTION "cc_add_python_requirements_cache(${TARGET})"
    VENV "${arg_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  cc_get_value_or_default(staging_dir arg_STAGING_DIR "${TARGET}")
  cmake_path(ABSOLUTE_PATH staging_dir
    BASE_DIRECTORY "${PYTHON_DISTCACHE_DIR}")

  cmake_path(APPEND staging_dir ".${TARGET}"
    OUTPUT_VARIABLE staging_stamp)

  if (arg_ALL)
    set(install ON)
  elseif (arg_INSTALL_CONDITION)
    set(install ${${arg_INSTALL_CONDITION}})
  elseif(arg_INSTALL_COMPONENT)
    set(install ON)
  else()
    set(install OFF)
  endif()

  cc_get_optional_keyword(ALL install)
  add_custom_target(${TARGET} ${ALL}
    DEPENDS "${staging_stamp}"
  )

  if(arg_REQUIREMENTS_DEPS)
    add_dependencies(${TARGET} ${arg_REQUIREMENTS_DEPS})
  endif()


  #-----------------------------------------------------------------------------
  # Build distributions list and invoke `pip wheel`.

  cc_get_python_requirements_list(
    REQUIREMENTS "${arg_REQUIREMENTS}"
    REQUIREMENTS_FILES "${arg_REQUIREMENTS_FILES}"
    OUTPUT_VARIABLE distributions
  )

  if(distributions)
    message(VERBOSE
      "Adding requirements target ${TARGET}, command: "
      "pip wheel ${PIP_QUIET} --wheel-dir '${staging_dir}' ${distributions}")

    add_custom_command(
      OUTPUT "${staging_stamp}"
      DEPENDS ${arg_REQUIREMENTS_FILES}
      COMMENT "${TARGET}: Retrieving Python distributions"
      COMMAND_EXPAND_LISTS
      VERBATIM

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${staging_dir}"
      COMMAND "${python}" -m pip wheel ${PIP_QUIET} --wheel-dir "${staging_dir}" ${distributions}
      COMMAND "${CMAKE_COMMAND}" -E touch "${staging_stamp}"
    )

    #-----------------------------------------------------------------------------
    # Set a couple of target properties used by downstream dependents

    set_target_properties(${TARGET} PROPERTIES
      python_distributions "${distributions}"
      python_distributions_cache_dir "${staging_dir}"
    )

    if(install AND arg_INSTALL_COMPONENT)
      cc_get_value_or_default(
        wheels_install_dir
        arg_INSTALL_DIR
        "${PYTHON_WHEELS_INSTALL_DIR}/${TARGET}")

      set_target_properties(${TARGET} PROPERTIES
        python_distributions_install_dir "${wheels_install_dir}"
      )

      install(
        DIRECTORY "${staging_dir}/"
        DESTINATION "${wheels_install_dir}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
        FILES_MATCHING PATTERN "*.whl"
      )
    endif()

  else()
    message(SEND_ERROR "cc_add_python_requirements_cache(${TARGET}) needs "
      "REQUIREMENTS, REQUIREMENTS_FILES, or both")
  endif()

endfunction()



#-------------------------------------------------------------------------------
## @fn cc_populate_python_venv
## @brief
##   Install required Python distributions into a local virtual environment

function(cc_populate_python_venv TARGET)
  set(_options
    ALL                 # Make this target a dependency of `all`
  )
  set(_singleargs
    VENV_TARGET         # Virtualenv specified by build target (preferred)
    VENV_PATH           # Virtualenv path if VENV_TARGET is not specified
  )
  set(_multiargs
    REQUIREMENTS_FILES  # Files containing package names to install
    REQUIREMENTS        # Add prerequisite target to obtain specific whels
    REQUIREMENTS_DEPS   # Upstream distribution cache targets on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_VENV_TARGET)
    get_target_property(venv_path ${arg_VENV_TARGET} local_path)

    if(NOT venv_path)
      message(FATAL_ERROR "Target cc_populate_python_venv(${TARGET}) "
        "VENV dependency '${arg_VENV_TARGET}' does not look like a build "
        "target added by `cc_add_python_venv()`, because it is missing "
        "the `local_path` property.")
    endif()

  elseif(arg_VENV_PATH)
    cmake_path(ABSOLUTE_PATH arg_VENV_PATH
      BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE venv_path)

  else()
    message(FATAL_ERROR "Target cc_populate_python_venv(${TARGET}) "
      "requires VENV_TARGET or VENV_PATH")
  endif()

  #-----------------------------------------------------------------------------
  # Create build target

  cmake_path(APPEND venv_path "bin" "python"  OUTPUT_VARIABLE venv_python)
  cmake_path(APPEND venv_path ".build-stamps" OUTPUT_VARIABLE stamp_dir)
  cmake_path(APPEND stamp_dir "${TARGET}"     OUTPUT_VARIABLE target_stamp)

  ### Creaate build target

  cc_get_optional_keyword(ALL arg_ALL)
  add_custom_target(${TARGET} ${ALL}
    DEPENDS ${target_stamp} ${arg_REQUIREMENTS_FILES}
  )

  if(arg_VENV_TARGET)
    add_dependencies(${TARGET} ${arg_VENV_TARGET})
  endif()

  set_target_properties(${TARGET} PROPERTIES
    local_path ${venv_path}
    target_stamp ${target_stamp}
  )


  #-------------------------------------------------------------------------------
  # Install any specified requirements:
  #   - Download and install packages listed explicitly in the REQUIREMENTS argument,
  #     or within files listed in the REQUIREMENTS_FILES argument
  #   - Install previously-downloaded packages from build targets listed in
  #     in the REQUIREMENTS_DEPS argument (each of which should have been
  #     added using `cc_add_python_requirements_cache()`, above).

  if (arg_REQUIREMENTS OR arg_REQUIREMENTS_FILES OR arg_REQUIREMENTS_DEPS)
    cmake_path(RELATIVE_PATH venv_path
      BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE venv_rel_path)

    add_custom_command(
      OUTPUT "${target_stamp}"
      COMMENT "${TARGET}: Populating Python Virtual Environment: ${venv_rel_path}"
      VERBATIM
    )

    if(arg_REQUIREMENTS OR arg_REQUIREMENTS_FILES)
      cc_get_python_requirements_list(
        REQUIREMENTS "${arg_REQUIREMENTS}"
        REQUIREMENTS_FILES "${arg_REQUIREMENTS_FILES}"
        OUTPUT_VARIABLE requirements
      )

      add_custom_command(
        OUTPUT "${target_stamp}" APPEND
        COMMAND "${venv_python}" -m pip install ${PIP_QUIET} --no-warn-script-location ${requirements}
      )
    endif()


    # Obtain locations of downloaded wheels from targets listed in `REQUIREMENTS_DEPS`.
    # Install any discovered wheels into this virtual environment.

    if(arg_REQUIREMENTS_DEPS)
      set(requirements_target ${arg_REQUIREMENTS_DEPS})
      add_dependencies(${TARGET} ${requirements_target})

      cc_get_target_property_recursively(
        TARGETS ${requirements_target}
        PROPERTY python_distributions_cache_dir
        OUTPUT_VARIABLE cache_dirs
      )

      foreach(cache_dir ${cache_dirs})
        add_custom_command(
          OUTPUT "${target_stamp}" APPEND
          COMMAND find "${cache_dir}"
          -name *.whl
          -exec "${venv_python}" -m pip install ${PIP_QUIET} --no-index --no-warn-script-location {} +
        )
      endforeach()
    endif()
  endif()

endfunction()




function(cc_add_python_venv_install_hook)
  set(_options
    VERBOSE             # Create VENV without `--quiet` option
  )
  set(_singleargs
    VENV_PATH           # Target `virtualenv` folder.
    COMPONENT           # CPack component to which hook is added
    POSTINST_TEMPLATE   # Use custom template for `postinst` script
    PRERM_TEMPLATE      # Use custom template for `prerm` script
  )
  set(_multiargs
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  #-----------------------------------------------------------------------------
  # Assign variable values for control scripts.
  # See `../debian/venv-postinst.in` and `../debian/venv-prerm.in`.

  cc_get_ternary(VERBOSE
    arg_VERBOSE
    true
    false)

  # `VENV_PATH` is the absolute path name to the environment we are populating
  # (This may be shared with additional wheels or it may be created new).
  cmake_path(ABSOLUTE_PATH arg_VENV_PATH
    BASE_DIRECTORY "${PYTHON_VENV_ROOT}"
    OUTPUT_VARIABLE VENV_PATH)


  #-----------------------------------------------------------------------------
  # Generate `postinst` and `prerm` scripts

  cc_get_argument_or_default(postinst_template
    arg_POSTINST_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/venv-postinst.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  cc_get_argument_or_default(prerm_template
    arg_PRERM_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/venv-prerm.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  set(script_name "venv")

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

