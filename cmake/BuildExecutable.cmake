## -*- cmake -*-
#===============================================================================
## @file BuildExecutable.cmake
## @brief Wrapper function for building executables
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

#===============================================================================
## @fn BuildExecutable

function(BuildExecutable TARGET)
  set(_options INSTALL)
  set(_singleargs DESTINATION INSTALL_COMPONENT)
  set(_multiargs SOURCES LIB_DEPS OBJ_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  foreach(_dep ${arg_MOD_DEPS})
    find_package("${_dep}" REQUIRED)
  endforeach()

  add_executable("${TARGET}" ${arg_SOURCES})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} PRIVATE ${arg_LIB_DEPS} ${arg_OBJ_DEPS})

  if(arg_PKG_DEPS)
    include(pkgconf)
    add_package_dependencies("${TARGET}"
      LIB_TYPE STATIC
      DEPENDS ${arg_PKG_DEPS}
    )
  endif()

  set(install_args
    TARGETS "${TARGET}"
    RUNTIME
  )

  if(arg_INSTALL_COMPONENT)
    set(install ON)
    list(APPEND install_args COMPONENT ${arg_INSTALL_COMPONENT})

  elseif(CPACK_CURRENT_COMPONENT)
    set(install ON)
    list(APPEND install_args COMPONENT ${CPACK_CURRENT_COMPONENT})

  elseif(arg_INSTALL)
    set(install ON)
    list(APPEND install_args COMPONENT common)

  else()
    set(install OFF)

  endif()

  if(arg_DESTINATION)
    list(APPEND install_args DESTINATION ${arg_DESTINATION})
  endif()

  if(install)
    install(${install_args})
  endif()
endfunction()
