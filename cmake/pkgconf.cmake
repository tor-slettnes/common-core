## -*- cmake -*-
#===============================================================================
## @file cc_pkgconf.cmake
## @brief Wrapper function for adding package dependencies
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

#===============================================================================
## @fn cc_pkgconf

function(cc_pkgconf PREFIX PACKAGE)
  set(_options REQUIRED)
  set(_singleargs)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  find_package(PkgConfig)
  if(PkgConfig_FOUND)
    pkg_check_modules(${PREFIX} "${PACKAGE}")
  else()
    message(WARNING
      "Unable to find 'pkg-config', which is needed to obtain information about "
      "available packages. Please install the 'cc_pkgconf' system package.")
  endif()

  if (arg_REQUIRED AND NOT ${PREFIX}_FOUND)
    string(TOLOWER ${PACKAGE} pkg)
    message(FATAL_ERROR
      "Unable to find '${PACKAGE}', which is required to build '${TARGET}'. "
      "Please install the required developer library on your system "
      "(try to search your package manager for '${pkg}-dev').")
  endif()
endfunction()


#===============================================================================
## @fn cc_add_package_dependencies

function(cc_add_package_dependencies TARGET)
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

  foreach (pkg ${arg_DEPENDS})
    cc_pkgconf(PKG ${pkg} REQUIRED)

    target_include_directories("${TARGET}" SYSTEM ${_include_scope} ${PKG_INCLUDE_DIRS})
    if(USE_STATIC_LIBS)
      link_directories(${PKG_STATIC_LIBRARY_DIRS})
      target_link_libraries("${TARGET}" ${_link_scope} ${PKG_STATIC_LIBRARIES})
    else()
      link_directories(${PKG_LIBRARY_DIRS})
      target_link_libraries("${TARGET}" ${_link_scope} ${PKG_LIBRARIES})
    endif()
  endforeach()
endfunction()

#===============================================================================
## @fn cc_add_external_dependency
##
## THE CODE BELOW IS EXPERIMENTAL.
## While it may work for some thrid-party software, it is by no means robus.

function(cc_add_external_dependency TARGET SUITE)
  set(_options INCLUDE_DIRS LIBRARY_DIRS LIBRARIES)
  set(_singleargs)
  set(_multiargs PKGCONF_TARGETS BUILD_TARGETS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" "${ARGN}")

  ### We have found nothing yet
  set(_found FALSE)

  ### First we try to find the required copmonents via PkgConfig
  find_package(PkgConfig)
  if(PkgConfig_FOUND)

    set(_found TRUE)
    set(_include_dirs)
    set(_link_dirs)
    set(_libs)

    if(arg_PGKCONF_TARGETS)
      set(_components ${arg_PKGONF_TARGETS})
    else()
      set(_components ${SUITE})
    endif()

    foreach(_component ${_components})
      pkg_check_modules(pkg ${_component})
      if (pkg_FOUND)
        list(APPEND _include_dirs "${pkg_INCLUDE_DIRS}")
        list(APPEND _link_dirs "${pkg_LIBRARY_DIRS}")
        list(APPEND _libs "${pkg_LIBRARIES}")
      else()
        set(_found FALSE)
      endif()
    endforeach()
  endif()

  ### If we failed so far, perhaps we have a generated configuration from a
  ### previous build
  if(NOT _found)
    set(_include_dirs)
    set(_link_dirs)
    set(_libs)

    find_package("${SUITE}" CONFIG)
    if (${SUITE}_FOUND)
      set(_found TRUE)
      foreach (_component ${_components})
        list(APPEND _include_dirs
          "$<TARGET_PROPERTY:${_component},INTERFACE_INCLUDE_DIRECTORIES>")

        list(APPEND _link_dirs
          "$<TARGET_PROPERTY:${_component},INTERFACE_LINK_DIRECTORIES>")

        list(APPEND _libs "${_component}"
          "$<TARGET_PROPERTY:${_component},INTERFACE_LINK_DIRECTORIES>")
      endforeach()
    endif()
  endif()

  ### If we failed to find all required components, retrieve the package/suite
  ### via `FetchContent_MakeAvaialble(${SUITE})`.  This requires a
  ### corresponding `FetchContent_Declare(${SUITE} ...)` statement, e.g.  in
  ### the `thirdparty/` folder.

  if(NOT _found)
    set(_include_dirs)
    set(_link_dirs)
    set(_libs)

    set(FETCHCONTENT_QUIET OFF)
    include(FetchContent)
    FetchContent_MakeAvailable(${SUITE})

    if(arg_BUILD_TARGETS)
      set(_components ${arg_BUILD_TARGETS})
    else()
      set(_components ${SUITE})
    endif()

    foreach(_component ${_components})
      if(TARGET "${_component}")
        list(APPEND _include_dirs
          "$<TARGET_PROPERTY:${_component},INTERFACE_INCLUDE_DIRECTORIES>")

        list(APPEND _link_dirs
          "$<TARGET_PROPERTY:${_component},INTERFACE_LINK_DIRECTORIES>")

        list(APPEND _libs
          "${_component}"
          "$<TARGET_PROPERTY:${_component},INTERFACE_LIBRARIES>")
      else()
        message(WARNING "External package ${SUITE} does not contain a '${_component}' target")
      endif()
    endforeach()
  endif()

  set(${SUITE}_INCLUDE_DIRS "${_include_dirs}" PARENT_SCOPE)
  set(${SUITE}_LIBRARY_DIRS "${_link_dirs}" PARENT_SCOPE)
  set(${SUITE}_LIBRARIES "${_libs}" PARENT_SCOPE)

  if(arg_INCLUDE_DIRS)
    message(DEBUG "Adding target ${TARGET} include directories: ${_include_dirs}")
    target_include_directories("${TARGET}" SYSTEM PUBLIC ${_include_dirs})
  endif()

  if(arg_LIBRARY_DIRS)
    message(DEBUG "Adding target ${TARGET} link directories: ${_link_dirs}")
    target_link_directories("${TARGET}" PUBLIC ${_link_dirs})
  endif()

  if(arg_LIBRARIES)
    message(DEBUG "Adding target ${TARGET} libraries: ${_libs}")
    target_link_libraries("${TARGET}" PUBLIC ${_libs})
  endif()
endfunction()

