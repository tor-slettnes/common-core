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

function(cc_add_python_venv TARGET)
  set(_options ALL)
  set(_singleargs
    VENV_PATH         # Virtualenv folder on local (build) machine
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  find_package(Python3
    COMPONENTS Interpreter
    REQUIRED)

  cc_get_value_or_default(venv_path arg_VENV_PATH "${TARGET}")
  cmake_path(ABSOLUTE_PATH venv_path
    BASE_DIRECTORY "${PYTHON_VENV_LOCAL_ROOT}")

  cmake_path(APPEND venv_path "bin" "python" OUTPUT_VARIABLE venv_python)

  #-----------------------------------------------------------------------------

  cc_get_optional_keyword(ALL arg_ALL)
  add_custom_target(${TARGET} ${ALL}
    DEPENDS ${venv_python})

  set_target_properties(${TARGET} PROPERTIES
    venv_path "${venv_path}"
    venv_python "${venv_python}"
  )

  #-----------------------------------------------------------------------------
  # Create the virtual environment

  add_custom_command(
    OUTPUT "${venv_python}"
    COMMENT "Creating Python Virtual Environment: ${venv_path}"
    VERBATIM

    COMMAND "${Python3_EXECUTABLE}" -m virtualenv ${PIP_QUIET} "${venv_path}"
  )
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
    BASE_DIRECTORY "${PYTHON_DISTCACHE_DIR}")

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
      COMMENT "Retrieving Python distributions for target: ${TARGET}"

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
    message(SEND_ERROR "cc_add_python_distributions_cache(${TARGET}) needs "
      "REQUIREMENTS_FILE, DISTRIBUTIONS, or both")
  endif()

endfunction()



#-------------------------------------------------------------------------------
## @fn cc_populate_python_venv
## @brief
##   Install required Python distributions into a local virtual environment

function(cc_populate_python_venv TARGET)
  set(_options
    ALL               # Make this target a dependency of `all`
  )
  set(_singleargs
    VENV_TARGET       # Virtualenv specified by build target (preferred)
    VENV_PATH         # Virtualenv path if VENV_TARGET is not specified
    REQUIREMENTS_FILE # Add upstream target to obtian wheels from this file
  )
  set(_multiargs
    DISTRIBUTIONS     # Add prerequisite target to obtain specific whels
    DISTCACHE_DEPS    # Upstream targets from which we obtain `.whl` files to install
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_VENV_TARGET)
    get_target_property(venv_path ${arg_VENV_TARGET} venv_path)

    if(NOT venv_path)
      message(FATAL_ERROR "Target cc_populate_python_venv(${TARGET}) "
        "VENV dependency '${arg_VENV_DEPENDS}' does not look like a build "
        "target added by `cc_add_python_venv()`, because it is missing "
        "the `venv_path` property.")
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
  add_custom_target(${TARGET} ${ALL}  DEPENDS ${target_stamp})
  add_dependencies(${TARGET} ${arg_VENV_TARGET})


  #-------------------------------------------------------------------------------
  # Obtain locations of downloaded wheels from targets listed in `DISTCACHE_DEPS`.
  # Install any discovered wheels into this virtual environment.

  if(arg_DISTRIBUTIONS OR arg_REQUIREMENTS_FILE)
    set(requirements_target ${TARGET}-requirements)
    cc_add_python_distributions_cache(${requirements_target}
      STAGING_DIR ${TARGET}
      REQUIREMENTS_FILE ${arg_REQUIREMENTS_FILE}
      DISTRIBUTIONS ${arg_DISTRIBUTIONS}
      DISTCACHE_DEPS ${arg_DISTCACHE_DEPS}
    )
  elseif(arg_DISTCACHE_DEPS)
    set(requirements_target ${arg_DISTCACHE_DEPS})

  else()
    message(FATAL_ERROR "Target cc_populate_python_venv(${TARGET}) "
      "requires DISTCACHE_DEPS, DISTRIBUTIONS, and/or REQUIIREMENTS_FILE")
  endif()

  add_dependencies(${TARGET} ${requirements_target})

  add_custom_command(
    OUTPUT "${target_stamp}"
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

endfunction()


