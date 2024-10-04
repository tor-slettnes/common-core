## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildPackage)
set(_service_template_dir ${CMAKE_CURRENT_LIST_DIR}/debian)

#===============================================================================
# @fn InstallDebianService
# @brief Create, install and optionally enable a SystemD service unit

function(InstallDebianService UNIT)
  set(_options USER ENABLE)
  set(_singleargs PROGRAM DESCRIPTION INSTALL_COMPONENT)
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

  if(arg_INSTALL_COMPONENT)
    set(_component "${arg_INSTALL_COMPONENT}")
  else()
    set(_component "COMMON")
  endif()

  if(NOT arg_PROGRAM)
    message(SEND_ERROR "InstallDebianService() missing required argument PROGRAM")
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

  set(_service_file "${CMAKE_CURRENT_BINARY_DIR}/${_service_unit}")
  configure_file(
    "${_service_template_dir}/service.in"
    "${_service_file}"
  )

  install(
    FILES "${_service_file}"
    DESTINATION "${_dest}"
    COMPONENT "${_component}"
  )

  if(arg_ENABLE)
    AddEnableHooks("${_service_unit}"
      INSTALL_DIRECTORY "${_install_root}/${_dest}"
      INSTALL_COMPONENT "${_component}"
    )
  endif()
endfunction()


#===============================================================================
# @fn AddEnableHooks
# @brief Add post-install and pre-removal hooks to enable/disable service unit

function(AddEnableHooks UNIT)
  set(_options)
  set(_singleargs INSTALL_DIRECTORY INSTALL_COMPONENT)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  #=============================================================================
  # Next follows some logic to create a `postinst` and `postrm` script to
  # enable/disable the systemd service unit during install/remove/upgrade.
  #
  # First we determine if we have already done this for other service units
  # within the same `.deb` package.  We do this by
  #    (a) using a unique staging directory for each package component,
  #        if and only if CPACK_DEB_COMPONENT_INSTALL is set
  #    (b) otherwise, maintaining an associated directory property to keep
  #        track of this and prior services associated with this directory
  #    (c) using this property to create a cumulative list of
  #        service units to be enabled/disabled.


  if(CPACK_DEB_COMPONENT_INSTALL)
    set(_staging_dir "${CMAKE_BINARY_DIR}/${arg_INSTALL_COMPONENT}")
    file(MAKE_DIRECTORY "${_staging_dir}")

    set(_service_units "${UNIT}")
  else()
    set(_staging_dir "${CMAKE_BINARY_DIR}")

    # Create or add this unit to the `service_units` directory property
    set_property(
      DIRECTORY "${_staging_dir}"
      APPEND
      PROPERTY service_units
      "${UNIT}"
    )

    get_property(_service_units
      DIRECTORY "${_staging_dir}"
      PROPERTY service_units
    )
  endif()

  # Define `SERVICE_UNITS` and `SERVICE_UNIT_PATHS` for `config_file()` 
  list(TRANSFORM _service_units
    PREPEND "${arg_INSTALL_DIRECTORY}/"
    OUTPUT_VARIABLE _service_unit_paths)

  list(JOIN _service_units " " SERVICE_UNITS)
  list(JOIN _service_unit_paths " " SERVICE_UNIT_PATHS)

  ### Add `postinst`/`prerm` hooks
  set(_postinst_file "${_staging_dir}/postinst")
  configure_file(
    "${_service_template_dir}/postinst.in"
    "${_postinst_file}"
  )

  set(_prerm_file "${_staging_dir}/prerm")
  configure_file(
    "${_service_template_dir}/prerm.in"
    "${_prerm_file}"
  )

  ### Add these generated hooks to the Debian package `control` file.
  if(CPACK_DEB_COMPONENT_INSTALL)
    CPackConfig("CPACK_DEBIAN_${arg_INSTALL_COMPONENT}_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  else()
    CPackConfig("CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  endif()

endfunction()
