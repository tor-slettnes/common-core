## -*- cmake -*-
#===============================================================================
## @file build_python_venv.cmake
## @brief Build a Python virtual environment (`virtualenv`)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


set(PYTHON_VENV
  "${PYTHON_OUT_DIR}/venv"
  CACHE STRING "Python virtual environment for building wheels & executables")

if(NOT IS_ABSOLUTE PYTHON_VENV)
  cmake_path(ABSOLUTE_PATH PYTHON_VENV
    BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PYTHON_VENV)
endif()


cmake_language(GET_MESSAGE_LOG_LEVEL log_level)
if(log_level MATCHES "^(TRACE|DEBUG|VERBOSE)$")
  unset(PIP_QUIET)
else()
  set(PIP_QUIET "--quiet")
endif()

#-------------------------------------------------------------------------------
# @fn cc_add_python_venv
# @brief
#     Create a Python virtual environment locally.
#
# This actually creates up to four build targets:
#
#  * An upstream `${TARGET}-create` is added to create the virtual environment
#    (using `python3 -m virtualenv`)
#  * If `DISTRIBUTIONS` or `REQUIREMENTS_FILE` are specified, an upstream target
#   `${TARGET}-requirements` is added to download/cache the specified wheels
#  * If either of those OR `DISTCACHE_DEPS` are specified, an upstream target
#   `${TARGET}-populate` is added to install the downloaded wheels
#  * The final build target `${TARGET}` simply depends on the above.

function(cc_add_python_venv TARGET)
  set(_options
    ALL               # Add wheel to the default default build target
  )
  set(_singleargs
    VENV_PATH         # Virtualenv folder on local (build) machine
    REQUIREMENTS_FILE # Add upstream target to obtian wheels from this file
    WHEELS_INSTALL_COMPONENT # CPack install component into which to add downloaded wheels
    WHEELS_INSTALL_DIR       # Override default install folder on target
  )
  set(_multiargs
    DISTRIBUTIONS     # Add upstream target to obtain specific whels
    DISTCACHE_DEPS    # Upstream targets from which we obtain `.whl` files to install
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  find_package(Python3
    COMPONENTS Interpreter
    REQUIRED)

  cc_get_value_or_default(venv_path arg_VENV_PATH "${TARGET}")
  cmake_path(ABSOLUTE_PATH venv_path
    BASE_DIRECTORY "${PYTHON_VENV}")

  cmake_path(APPEND venv_path "bin" "python" OUTPUT_VARIABLE venv_python)
  cmake_path(APPEND venv_path "build-stamps" OUTPUT_VARIABLE stamp_dir)
  cmake_path(APPEND stamp_dir "created"      OUTPUT_VARIABLE venv_stamp)
  cmake_path(APPEND stamp_dir "${TARGET}"    OUTPUT_VARIABLE target_stamp)

  #-----------------------------------------------------------------------------
  cc_get_optional_keyword(ALL arg_ALL)
  add_custom_target(${TARGET} ${ALL})

  set_target_properties(${TARGET} PROPERTIES
    venv_path "${venv_path}"
    venv_python "${venv_python}"
    stamp_dir "${stamp_dir}"
  )

  #-----------------------------------------------------------------------------
  # Create the virtual environment

  set(create_target ${TARGET}-create)
  add_custom_target(${create_target} DEPENDS ${venv_stamp})
  add_custom_command(
    OUTPUT "${venv_stamp}"
    COMMENT "Creating Python Virtual Environment: ${venv_path}"
    VERBATIM

    COMMAND "${Python3_EXECUTABLE}" -m virtualenv ${PIP_QUIET} "${venv_path}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${stamp_dir}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${venv_stamp}"
  )

  #-------------------------------------------------------------------------------
  # Obtain locations of downloaded wheels from targets listed in `DEPENDS`.
  # Install any discovered wheels into this virtual environment.

  if(arg_DISTRIBUTIONS OR arg_REQUIREMENTS_FILE)
    set(requirements_target ${TARGET}-requirements)
    cc_add_python_distributions_cache(${requirements_target}
      STAGING_DIR ${TARGET}
      REQUIREMENTS_FILE ${arg_REQUIREMENTS_FILE}
      DISTRIBUTIONS ${arg_DISTRIBUTIONS}
      DISTCACHE_DEPS ${arg_DISTCACHE_DEPS}
      INSTALL_COMPONENT ${arg_WHEELS_INSTALL_COMPONENT}
      INSTALL_DIR ${arg_WHEELS_INSTALL_DIR}
    )
  elseif(arg_DISTCACHE_DEPS)
    set(requirements_target ${arg_DISTCACHE_DEPS})
  endif()

  if(requirements_target)
    set(populate_target ${TARGET}-populate)
    add_custom_target(${populate_target} DEPENDS ${target_stamp})
    add_dependencies(${TARGET} ${populate_target})
    add_dependencies(${populate_target}
      ${create_target}
      ${requirements_target}
    )

    add_custom_command(
      OUTPUT "${target_stamp}"
      DEPENDS "${venv_stamp}"
      COMMENT "Populating Python Virtual Environment: ${venv_path}"
      VERBATIM
      COMMAND_EXPAND_LISTS
    )

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
        -exec "${venv_python}" -m pip ${PIP_QUIET} install --no-index --no-warn-script-location {} +
      )
    endforeach()

    add_custom_command(
      OUTPUT "${target_stamp}" APPEND
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${stamp_dir}"
      COMMAND "${CMAKE_COMMAND}" -E touch "${target_stamp}"
    )

  else()
    add_dependencies(${TARGET} ${create_target})
  endif()
endfunction()


#-------------------------------------------------------------------------------
## @fn cc_add_python_distributions_cache
## @brief
##   Download distributions

function(cc_add_python_distributions_cache TARGET)
  set(_options
    ALL                         # Add wheel to the default default build target
  )
  set(_singleargs
    REQUIREMENTS_FILE           # File containing required distribution names
    STAGING_DIR                 # Override default staging directory
    VENV                        # Use Python interpreter from a VENV (optional)
    INSTALL_COMPONENT           # Install cache onto target
    INSTALL_DIR                 # Override default installation folder
  )
  set(_multiargs
    DISTRIBUTIONS                # Additional Python distribution names
    DISTCACHE_DEPS               # Distribution cache targets on which we depend
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_find_python(
    ACTION "cc_add_python_distributions_cache(${TARGET})"
    VENV "${arg_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  cc_get_value_or_default(staging_dir arg_STAGING_DIR "${TARGET}")
  cmake_path(ABSOLUTE_PATH staging_dir
    BASE_DIRECTORY "${PYTHON_STAGING_ROOT}")

  cmake_path(APPEND staging_dir ".${TARGET}-stamp"
    OUTPUT_VARIABLE staging_stamp)

  cc_get_optional_keyword(ALL arg_ALL)
  add_custom_target(${TARGET} ${ALL}
    DEPENDS "${staging_stamp}"
  )

  if(arg_DISTCACHE_DEPS)
    add_dependencies(${TARGET} ${arg_DISTCACHE_DEPS})
  endif()


  #-----------------------------------------------------------------------------
  # Build distributions list and invoke `pip wheel`.

  unset(distributions)
  if(arg_REQUIREMENTS_FILE)
    file(STRINGS "${arg_REQUIREMENTS_FILE}" distributions)
  endif()
  list(APPEND distributions ${arg_DISTRIBUTIONS})

  if(distributions)
    add_custom_command(
      OUTPUT "${staging_stamp}"
      DEPENDS ${arg_REQUIREMENTS_FILE}
      COMMENT "Retrieving Python distributions: ${TARGET}"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${staging_dir}"
      COMMAND "${python}" -m pip ${PIP_QUIET} wheel --wheel-dir "${staging_dir}" ${distributions}
      COMMAND "${CMAKE_COMMAND}" -E touch "${staging_stamp}"
    )

    #-----------------------------------------------------------------------------
    # Set a couple of target properties used by downstream dependents

    set_target_properties(${TARGET} PROPERTIES
      python_distributions "${distributions}"
      python_distributions_cache_dir "${staging_dir}"
    )

    if(WITH_PYTHON_REQUIREMENTS AND arg_INSTALL_COMPONENT)
      cc_get_value_or_default(
        wheels_install_dir
        arg_INSTALL_DIR
        "${PYTHON_WHEELS_INSTALL_DIR}")

      install(
        DIRECTORY "${staging_dir}/"
        DESTINATION "${wheels_install_dir}"
        COMPONENT "${arg_INSTALL_COMPONENT}"
        FILES_MATCHING PATTERN "*.whl"
      )
    endif()

  else()
    message(SEND_ERROR "cc_add_python_distribution_cache(${TARGET}) needs "
      "REQUIREMENTS_FILE, DISTRIBUTIONS, or both")
  endif()

endfunction()



#-------------------------------------------------------------------------------
## @fn cc_add_venv_install_hook
## @brief
##   Add post-install/pre-rm hook to add/remove Python virtual environment

function(cc_add_venv_install_hook)
  set(_options
    ALLOW_DOWNLOADS     # Allow requirements to be downloaded on install
  )
  set(_singleargs
    WHEEL_NAME          # Base name of package/distribution to install
    WHEEL_VERSION       # Version of package/distribution to install
    WHEELS_INSTALL_DIR  # Directory in which to find required Python distributions
    VENV_PATH           # Target `virtualenv` folder.
    COMPONENT           # CPack component to which hook is added
    SYMLINKS_TARGET_DIR # Create symlinks in this folder (optional)
    POSTINST_TEMPLATE   # Use custom template for `postinst` script
    PRERM_TEMPLATE      # Use custom template for `prerm` script
  )
  set(_multiargs
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

  # `ALLOW_DOWNLOADS` allows downloading Python required distributions when creating `venv`
  cc_get_ternary(ALLOW_DOWNLOADS arg_ALLOW_DOWNLOADS true false)

  # `VENV_PATH` is the absolute path name to the environment we are populating
  # (This may be shared with additional wheels or it may be created new).
  cc_get_value_or_default(VENV_PATH
    arg_VENV_PATH
    ${PYTHON_VENV_ROOT}/${WHEEL_NAME})
  cmake_path(ABSOLUTE_PATH VENV_PATH
    BASE_DIRECTORY "${PYTHON_VENV_ROOT}")

  # `WHEELS_INSTALL_DIR` is where to find the wheel and its requirements
  cc_get_value_or_default(WHEELS_INSTALL_DIR
    arg_WHEELS_INSTALL_DIR
    ${PYTHON_WHEELS_INSTALL_DIR})
  cmake_path(ABSOLUTE_PATH WHEELS_INSTALL_DIR
    BASE_DIRECTORY "${INSTALL_ROOT}")

  # `SYMLINKS` is a list of executable scripts for which we want to create
  # symbolic links, e.g. in `/usr/bin` or `/usr/sbin`.
  list(JOIN arg_SYMLINKS " " SYMLINKS)

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
