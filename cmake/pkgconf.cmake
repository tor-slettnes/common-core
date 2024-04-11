## -*- cmake -*-
#===============================================================================
## @file pkgconf.cmake
## @brief Wrapper function for adding package dependencies
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

#===============================================================================
## @fn build_library

function(add_package_dependencies TARGET)
  set(_options)
  set(_singleargs LIB_TYPE)
  set(_multiargs DEPENDS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if("${arg_LIB_TYPE}" MATCHES "^(INTERFACE|OBJECT)$")
    set(_include_scope INTERFACE)
    set(_link_scope INTERFACE)
  else()
    set(_include_scope PUBLIC)
    set(_link_scope PRIVATE)
  endif()

  find_package(PkgConfig)
  if(PkgConfig_FOUND)
    foreach (pkg ${arg_DEPENDS})
      pkg_check_modules(PKG "${pkg}")
      if (NOT PKG_FOUND)
        message(FATAL_ERROR
          "Unable to find '${pkg}', which is required to build '${TARGET}'. "
          "Please install the required developer library on your system "
          "(try to search your package manager for '${pkg}-dev').")
      endif()

      link_directories(${PKG_LIBRARY_DIRS})
      target_include_directories("${TARGET}" SYSTEM ${_include_scope} ${PKG_INCLUDE_DIRS})
      target_link_libraries("${TARGET}" ${_link_scope} ${PKG_LIBRARIES})
    endforeach()
  else()
    message(WARNING
      "Unable to find 'pkg-config' on your system. This is needed to obtain "
      "information about the following packages, which are required to "
      "build ${TARGET} dependencies: ${PKG_DEPS}. Please install `pkgconf` "
      "on your system; meanwhile We'll proceed cautiously.")
  endif()
endfunction()
