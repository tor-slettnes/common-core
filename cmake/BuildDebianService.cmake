## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildPackage)
set(_service_template_dir ${CMAKE_CURRENT_LIST_DIR}/debian)

function(InstallDebianService UNIT)
  set(_options USER)
  set(_singleargs PROGRAM DESCRIPTION INSTALL_COMPONENT)
  set(_multiargs ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Install service unit: ${UNIT}.service

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
    set(SERVICE_UNIT "${UNIT}")
  else()
    set(SERVICE_UNIT "${UNIT}.service")
  endif()

  set(SERVICE_PROGRAM "${_program}")
  set(SERVICE_ARGS "${arg_ARGS}")
  set(SERVICE_DESCRIPTION "${arg_DESCRIPTION}")
  set(SERVICE_UNIT_PATH "${_install_root}/${_dest}/${SERVICE_UNIT}")
  set(_service_file "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_UNIT}")

  configure_file(
    "${_service_template_dir}/service.in"
    "${_service_file}"
  )

  install(
    FILES "${_service_file}"
    DESTINATION "${_dest}"
    COMPONENT "${_component}"
  )

  ### Add `postinst`/`prerm` hooks
  set(_postinst_file "${CMAKE_CURRENT_BINARY_DIR}/postinst")
  configure_file(
    "${_service_template_dir}/postinst.in"
    "${_postinst_file}"
  )

  set(_prerm_file "${CMAKE_CURRENT_BINARY_DIR}/prerm")
  configure_file(
    "${_service_template_dir}/prerm.in"
    "${_prerm_file}"
  )

  if(CPACK_DEB_COMPONENT_INSTALL)
    CPackConfig("CPACK_DEBIAN_${_component}_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  else()
    CPackConfig("CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  endif()

endfunction()

