## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


#===============================================================================
# @fn cc_add_debian_service
# @brief Create, install and optionally enable a SystemD service unit

function(cc_add_debian_service UNIT)
  set(_options USER ENABLE)
  set(_singleargs
    PROGRAM                     # Path to executable to launch as service/daemon
    DESCRIPTION                 # Description field for service unit
    USERNAME                    # Username for serivce
    INSTALL_COMPONENT           # CPack component containing service unit
    SERVICE_TEMPLATE            # Custom Service unit template
    PREINST_TEMPLATE            # Custom `preinst` script template
    POSTINST_TEMPLATE           # Custom `postinst` script template
    PRERM_TEMPLATE              # Custom `prerm` script template
    POSTRM_TEMPLATE             # Custom `postrm` script template

  )
  set(_multiargs ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  #-----------------------------------------------------------------------------
  # Determine paths for the systemd `.service` file

  if(arg_USER)
    set(_dest "lib/systemd/user")
  else()
    set(_dest "lib/systemd/system")
  endif()

  string(REGEX MATCH ".service$" SERVICE_SUFFIX "${UNIT}")
  if(SERVICE_SUFFIX)
    set(_service_unit "${UNIT}")
  else()
    set(_service_unit "${UNIT}.service")
  endif()


  # Populate some variables that are referenced in the `service-unit.in` template.

  if(NOT arg_PROGRAM)
    message(SEND_ERROR "cc_add_debian_service(${UNIT}) needs PROGRAM")
  else()
    cmake_path(APPEND INSTALL_ROOT "${arg_PROGRAM}"
      OUTPUT_VARIABLE SERVICE_PROGRAM)
  endif()

  cmake_path(APPEND INSTALL_ROOT ${SETTINGS_DIR}
    OUTPUT_VARIABLE SETTINGS_DIR)

  list(JOIN arg_ARGS " " SERVICE_ARGS)
  set(SERVICE_DESCRIPTION "${arg_DESCRIPTION}")
  set(SERVICE_USER "${arg_USERNAME}")

  cc_get_value_or_default(service_template
    arg_SERVICE_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/service-unit.in")
  configure_file("${service_template}" "${_service_unit}" @ONLY)

  if(arg_INSTALL_COMPONENT)
    install(
      FILES "${CMAKE_CURRENT_BINARY_DIR}/${_service_unit}"
      DESTINATION "${_dest}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()

  if(arg_ENABLE)
    cc_add_service_enable_hooks("${_service_unit}"
      INSTALL_DIRECTORY "${_dest}"
      INSTALL_COMPONENT "${arg_INSTALL_COMPONENT}"
      PREINST_TEMPLATE "${arg_PREINST_TEMPLATE}"
      POSTINST_TEMPLATE "${arg_POSTINST_TEMPLATE}"
      PRERM_TEMPLATE "${arg_PRERM_TEMPLATE}"
      POSTRM_TEMPLATE "${arg_POSTRM_TEMPLATE}"
    )
  endif()
endfunction()


#===============================================================================
# @fn cc_add_service_enable_hooks
# @brief Add post-install and pre-removal hooks to enable/disable service unit

function(cc_add_service_enable_hooks UNIT)
  set(_options)
  set(_singleargs
    INSTALL_DIRECTORY           # Folder where service units are located
    INSTALL_COMPONENT           # CPack component containing service unit
    PREINST_TEMPLATE            # Custom `preinst` script template
    POSTINST_TEMPLATE           # Custom `postinst` script template
    PRERM_TEMPLATE              # Custom `prerm` script template
    POSTRM_TEMPLATE             # Custom `postrm` script template
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_argument_or_default(postinst_template
    arg_POSTINST_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/service-postinst.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  cc_get_argument_or_default(prerm_template
    arg_PRERM_TEMPLATE
    "${DEBIAN_TEMPLATE_DIR}/service-prerm.in"
    "${arg_KEYWORDS_MISSING_VALUES}")

  set(SERVICE_UNIT "${UNIT}")
  cmake_path(APPEND INSTALL_ROOT "${arg_INSTALL_DIRECTORY}" "${UNIT}"
    OUTPUT_VARIABLE SERVICE_UNIT_PATH)

  cmake_path(GET UNIT STEM LAST_ONLY unitbase)
  set(script "${unitbase}-service")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "preinst"
    TEMPLATE "${arg_PREINST_TEMPLATE}"
    OUTPUT_FILE "70-${script}")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "postinst"
    TEMPLATE "${postinst_template}"
    OUTPUT_FILE "70-${script}")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "prerm"
    TEMPLATE "${prerm_template}"
    OUTPUT_FILE "30-${script}")

  cc_add_debian_control_script(
    COMPONENT "${arg_INSTALL_COMPONENT}"
    PHASE "postrm"
    TEMPLATE "${arg_POSTRM_TEMPLATE}"
    OUTPUT_FILE "30-${script}")

endfunction()
