## -*- cmake -*-
#===============================================================================
## @file BuildPackage.cmake
## @brief CMake include file to create Debian packages
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(BuildPackage PACKAGE_NAME)
  set(_options)
  set(_singleargs INSTALL_PREFIX VENDOR MAINTAINER SECTION DESCRIPTION ARCH)
  set(_multiargs DEPENDS RECOMMENDS SUGGESTS ENHANCES CONFLICTS PROVIDES REPLACES)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

  if(arg_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "${arg_INSTALL_PREFIX}")
  elseif(CPACK_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "${CPACK_INSTALL_PREFIX}")
  elseif(NOT CPACK_PACKAGING_INSTALL_PREFIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/cc")
   endif()

  if(arg_VENDOR)
    set(CPACK_PACKAGE_VENDOR "${arg_VENDOR}")
  elseif(NOT CPACK_PACKAGE_VENDOR)
    set(CPACK_PACKAGE_VENDOR "Tor Slettnes")
  endif()

  if(arg_MAINTAINER)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${arg_VENDOR}")
  elseif(NOT CPACK_DEBIAN_PACKAGE_MAINTAINER)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tor Slettnes <tor@slett.net>")
  endif()

  if(arg_SECTION)
    set(CPACK_DEBIAN_PACKAGE_SECTION "${arg_SECTION}")
  elseif(NOT CPACK_DEBIAN_PACKAGE_SECTION)
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
  endif()

  if(arg_ARCH)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arg_ARCH}")
    set(CPACK_SYSTEM_NAME "${arg_ARCH}")

  elseif(CPACK_DEBIAN_PACKAGE_ARCHITECTURE AND NOT CPACK_SYSTEM_NAME)
    set(CPACK_SYSTEM_NAME ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

  elseif(NOT CPACK_SYSTEM_NAME)
    if (${CMAKE_CROSSCOMPILING})
      set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
    else()
      execute_process(
        COMMAND dpkg --print-architecture
        OUTPUT_VARIABLE CPACK_SYSTEM_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
  endif()

  if(arg_DESCRIPTION)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${arg_DESCRIPTION}")
  endif()

  foreach(arg ${_multiargs})
    if(arg_${arg})
      list(APPEND CPACK_DEBIAN_PACKAGE_${arg} ${arg_${arg}})
    endif()
  endforeach()

  include(CPack)
endfunction()
