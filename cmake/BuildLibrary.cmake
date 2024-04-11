## -*- cmake -*-
#===============================================================================
## @file BuildLibrary.cmake
## @brief Wrapper function for building libraries
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

#===============================================================================
## @fn BuildLibrary

function(BuildLibrary TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE)
  set(_multiargs SOURCES LIB_DEPS OBJ_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_LIB_TYPE)
    string(TOUPPER "${arg_LIB_TYPE}" _type)
  elseif(arg_SOURCES)
    set(_type STATIC)
  else()
    set(_type INTERFACE)
  endif()

  if(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  elseif("${_type}" MATCHES "^(INTERFACE|OBJECT)$")
    set(_scope INTERFACE)
  else()
    set(_scope PUBLIC)
  endif()

  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  foreach (_dep ${arg_MOD_DEPS})
    find_package("${_dep}" REQUIRED)
  endforeach()

  add_library("${TARGET}" ${_type} ${arg_SOURCES})
  target_include_directories(${TARGET} ${_scope} ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} ${_scope} ${arg_LIB_DEPS} ${arg_OBJ_DEPS})

  if(arg_PKG_DEPS)
    include(pkgconf)
    add_package_dependencies("${TARGET}"
      LIB_TYPE ${_type}
      DEPENDS ${arg_PKG_DEPS}
    )
  endif()
endfunction()
