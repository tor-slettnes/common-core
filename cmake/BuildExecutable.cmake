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
  set(_singleargs DESTINATION)
  set(_multiargs SOURCES LIB_DEPS OBJ_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  foreach (_dep ${arg_MOD_DEPS})
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

  if(arg_DESTINATION)
    install(
      TARGETS "${TARGET}"
      RUNTIME
      DESTINATION "${arg_DESTINATION}"
    )
  elseif(arg_INSTALL)
    install(
      TARGETS "${TARGET}"
      RUNTIME
    )
  endif()

endfunction()
