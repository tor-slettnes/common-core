## -*- cmake -*-
#===============================================================================
## @file cc_add_library.cmake
## @brief Wrapper function for building libraries
## @author Tor Slettnes
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

cc_get_ternary(DEFAULT_LIB_TYPE
  BUILD_CPP_STATIC_LIBS "STATIC" "SHARED")

#===============================================================================
## @fn cc_add_library

function(cc_add_library TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE)
  set(_multiargs SOURCES LIB_DEPS OBJ_DEPS PROTO_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_LIB_TYPE)
    string(TOUPPER "${arg_LIB_TYPE}" _type)
  elseif(arg_SOURCES)
    set(_type ${DEFAULT_LIB_TYPE})
  else()
    set(_type INTERFACE)
  endif()

  if(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  elseif(_type MATCHES "^(INTERFACE|OBJECT)$")
    set(_scope INTERFACE)
  else()
    set(_scope PUBLIC)
  endif()

  list(TRANSFORM arg_PROTO_DEPS
    APPEND "_cpp"
    OUTPUT_VARIABLE proto_deps
  )

  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  foreach(_dep ${arg_MOD_DEPS})
    find_package("${_dep}" REQUIRED)
  endforeach()

  cc_get_ternary(exclude_from_all BUILD_ALL_LIBS "" "EXCLUDE_FROM_ALL")
  add_library("${TARGET}" ${_type} ${exclude_from_all} ${arg_SOURCES})
  target_include_directories(${TARGET} ${_scope} ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} ${_scope} ${arg_LIB_DEPS} ${arg_OBJ_DEPS} ${proto_deps})

  if(arg_PKG_DEPS)
    cc_add_package_dependencies("${TARGET}"
      LIB_TYPE ${_type}
      DEPENDS ${arg_PKG_DEPS}
    )
  endif()
endfunction()
