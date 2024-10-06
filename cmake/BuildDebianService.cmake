## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildPackage)
set(SERVICE_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR}/debian)

#===============================================================================
# @fn InstallDebianService
# @brief Create, install and optionally enable a SystemD service unit

function(InstallDebianService UNIT)
  set(_options USER ENABLE)
  set(_singleargs PROGRAM DESCRIPTION
    INSTALL_COMPONENT INSTALL_GROUP
    SERVICE_TEMPLATE PREINST_TEMPLATE POSTINST_TEMPLATE PRERM_TEMPLATE POSTRM_TEMPLATE
  )
  set(_multiargs ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  #=============================================================================
  # Determine paths for the systemd `.service` file

  if(CPACK_PACKAGING_INSTALL_PREFIX)
    set(_install_root "${CPACK_PACKAGING_INSTALL_PREFIX}")
  else()
    set(_install_root "/usr")
  endif()

  if(arg_USER)
    set(_dest "lib/systemd/user")
  else()
    set(_dest "lib/systemd/system")
  endif()

  if(NOT arg_INSTALL_COMPONENT)
    message(SEND_ERROR "InstallDebianService(${UNIT}) needs INSTALL_COMPONENT")
  endif()

  if(NOT arg_INSTALL_GROUP)
    message(SEND_ERROR "InstallDebianService(${UNIT}) needs INSTALL_GROUP")
  endif()

  if(NOT arg_PROGRAM)
    message(SEND_ERROR "InstallDebianService(${UNIT}) needs PROGRAM")
  elseif(IS_ABSOLUTE "${arg_PROGRAM}")
    set(_program "${arg_PROGRAM}")
  else()
    set(_program "${_install_root}/${arg_PROGRAM}")
  endif()

  string(REGEX MATCH ".service$" SERVICE_SUFFIX "${UNIT}")
  if(SERVICE_SUFFIX)
    set(_service_unit "${UNIT}")
  else()
    set(_service_unit "${UNIT}.service")
  endif()

  set(SERVICE_PROGRAM "${_program}")
  set(SERVICE_ARGS "${arg_ARGS}")
  set(SERVICE_DESCRIPTION "${arg_DESCRIPTION}")

  add_debian_file_from_template(
    TEMPLATE_VARIABLE arg_SERVICE_TEMPLATE
    DEFAULT_TEMPLATE "${SERVICE_TEMPLATE_DIR}/service.in"
    OUTPUT_FILE "${_service_unit}"
  )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${_service_unit}"
    DESTINATION "${_dest}"
    COMPONENT "${arg_INSTALL_COMPONENT}"
  )

  if(arg_ENABLE)
    AddEnableHooks("${_service_unit}"
      INSTALL_DIRECTORY "${_install_root}/${_dest}"
      INSTALL_COMPONENT "${arg_INSTALL_COMPONENT}"
      INSTALL_GROUP "${arg_INSTALL_GROUP}"
      PREINST_TEMPLATE "${arg_PREINST_TEMPLATE}"
      POSTINST_TEMPLATE "${arg_POSTINST_TEMPLATE}"
      PRERM_TEMPLATE "${arg_PRERM_TEMPLATE}"
      POSTRM_TEMPLATE "${arg_POSTRM_TEMPLATE}"
    )
  endif()
endfunction()


#===============================================================================
# @fn AddEnableHooks
# @brief Add post-install and pre-removal hooks to enable/disable service unit

function(AddEnableHooks UNIT)
  set(_options)
  set(_singleargs INSTALL_DIRECTORY INSTALL_COMPONENT INSTALL_GROUP)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  #=============================================================================
  # Next follows some logic to create a `postinst` and `postrm` script to
  # enable/disable the systemd service unit during install/remove/upgrade.
  #
  # First we determine if we have already done this for other service units
  # within the same `.deb` package.  We do this by
  #    (a) using a unique staging directory for each package component or group,
  #        if and only if CPACK_DEB_COMPONENT_INSTALL is set
  #    (b) otherwise, maintaining an associated directory property to keep
  #        track of this and prior services associated with this directory
  #    (c) using this property to create a cumulative list of
  #        service units to be enabled/disabled.

  # Obtain the CPack Debian grouping name into `staging_group`
  get_cpack_debian_grouping(
    PREFIX "service_units"
    GLUE "_"
    COMPONENT "${arg_INSTALL_COMPONENT}"
    GROUP "${arg_INSTALL_GROUP}"
    OUTPUT_VARIABLE staging_group)

  # Add this to any previous units in the same group
  set(_service_units "$ENV{${staging_group}}")
  list(APPEND _service_units "${UNIT}")
  set(ENV{${staging_group}} "${_service_units}")

  # Set up a staging directory for this package
  cmake_path(APPEND CMAKE_BINARY_DIR "${staging_group}"
    OUTPUT_VARIABLE _staging_dir)

  file(MAKE_DIRECTORY "${_staging_dir}")

  # Define `SERVICE_UNITS` and `SERVICE_UNIT_PATHS` for `config_file()` 
  list(TRANSFORM _service_units
    PREPEND "${arg_INSTALL_DIRECTORY}/"
    OUTPUT_VARIABLE _service_unit_paths)

  list(JOIN _service_units " " SERVICE_UNITS)
  list(JOIN _service_unit_paths " " SERVICE_UNIT_PATHS)

  ### Add install/remove hooks
  set(extra_control)
  add_debian_file_from_template(
    TEMPLATE_VARIABLE arg_PREINST_TEMPLATE
    OUTPUT_FILE "${_staging_dir}/preinst"
    EXTRA_CONTROL_VARIABLE extra_control)

  add_debian_file_from_template(
    TEMPLATE_VARIABLE arg_POSTINST_TEMPLATE
    DEFAULT_TEMPLATE "${SERVICE_TEMPLATE_DIR}/postinst.in"
    OUTPUT_FILE "${_staging_dir}/postinst"
    EXTRA_CONTROL_VARIABLE extra_control)

  add_debian_file_from_template(
    TEMPLATE_VARIABLE arg_PRERM_TEMPLATE
    DEFAULT_TEMPLATE "${SERVICE_TEMPLATE_DIR}/prerm.in"
    OUTPUT_FILE "${_staging_dir}/prerm"
    EXTRA_CONTROL_VARIABLE extra_control)

  add_debian_file_from_template(
    TEMPLATE_VARIABLE arg_POSTRM_TEMPLATE
    OUTPUT_FILE "${_staging_dir}/postrm"
    EXTRA_CONTROL_VARIABLE extra_control)

  ### Add these generated hooks to the Debian package `control` file.
  CPackDebianConfig(PACKAGE_CONTROL_EXTRA "${extra_control}"
    COMPONENT "${arg_INSTALL_COMPONENT}"
    GROUP "${arg_INSTALL_GROUP}"
    APPEND)
endfunction()


function(add_debian_file_from_template)
  set(_options)
  set(_singleargs TEMPLATE_VARIABLE DEFAULT_TEMPLATE OUTPUT_FILE EXTRA_CONTROL_VARIABLE)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(${arg_TEMPLATE_VARIABLE})
    set(template_file "${${arg_TEMPLATE_VARIABLE}}")
  elseif(arg_DEFAULT_TEMPLATE)
    set(template_file "${arg_DEFAULT_TEMPLATE}")
  else()
    return()
  endif()

  if (arg_OUTPUT_FILE)
    configure_file("${template_file}" "${arg_OUTPUT_FILE}")
  endif()

  if (arg_EXTRA_CONTROL_VARIABLE)
    set(extra_control "${${arg_EXTRA_CONTROL_VARIABLE}}")
    list(APPEND extra_control "${arg_OUTPUT_FILE}")
    set("${arg_EXTRA_CONTROL_VARIABLE}" "${extra_control}" PARENT_SCOPE)
  endif()
endfunction()
