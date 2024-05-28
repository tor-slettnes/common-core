## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(BuildPackage)
set(_service_template_dir ${CMAKE_CURRENT_LIST_DIR}/debian)

function(InstallServiceUnit UNIT)
  set(_options USER)
  set(_singleargs PROGRAM DESCRIPTION COMPONENT)
  set(_multiargs ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Install service unit: ${UNIT}.service 

  if(arg_USER)
    set(_dest "lib/systemd/user")
  else()
    set(_dest "lib/systemd/system")
  endif()

  if(arg_COMPONENT)
    string(TOUPPER "${arg_COMPONENT}" _component)
  else()
    set(_component "COMMON")
  endif()

  if(NOT arg_PROGRAM)
    message(SEND_ERROR "InstallServiceUnit() missing required argument PROGRAM")
  endif()

  string(REGEX MATCH ".service$" SERVICE_SUFFIX "${UNIT}")
  if(SERVICE_SUFFIX)
    set(SERVICE_UNIT "${UNIT}")
  else()
    set(SERVICE_UNIT "${UNIT}.service")
  endif()

  set(SERVICE_PROGRAM "${arg_PROGRAM}")
  set(SERVICE_ARGS "${arg_ARGS}")
  set(SERVICE_DESCRIPTION "${arg_DESCRIPTION}")
  set(_service_file "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_UNIT}")

  configure_file(
    "${_service_template_dir}/service.in"
    "${_service_file}"
  )

  install(
    FILES "${_service_file}"
    DESTINATION "${_dest}"
    COMPONENT "${arg_COMPONENT}"
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

  if(arg_COMPONENT AND CPACK_DEB_COMPONENT_INSTALL)
    CPackConfig("CPACK_DEBIAN_${_component}_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  else()
    CPackConfig("CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA"
      "${_postinst_file};${_prerm_file}"
    )
  endif()

endfunction()

