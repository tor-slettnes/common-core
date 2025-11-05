## -*- cmake -*-
#===============================================================================
## @file cc_add_executable.cmake
## @brief Wrapper function for building executables
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
#===============================================================================

#===============================================================================
## @fn cc_add_executable

function(cc_add_executable TARGET)
  set(_options)
  set(_singleargs
    DESTINATION                 # Destination directory, e.g. `bin` or `sbin`
    INSTALL                     # Whether to install (requires INSTALL_COMPONENT; default is ON)
    INSTALL_COMPONENT           # Install component; required for install.
  )
  set(_multiargs
    SOURCES                     # Input files
    LIB_DEPS                    # CMake target or third-party library dependencies
    OBJ_DEPS                    # CMake "OBJECT" library dependencies
    PKG_DEPS                    # 3rd party package dependencies; requires `pkg-conf`.
    MOD_DEPS                    # CMake `Find*` module dependencies
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  foreach(_dep ${arg_MOD_DEPS})
    find_package("${_dep}" REQUIRED)
  endforeach()

  cc_get_argument_or_default(install
    arg_INSTALL
    "${arg_INSTALL_COMPONENT}"
    "${arg_KEYWORDS_MISSING_VALUES}")

  if (install OR BUILD_ALL_BINARIES)
    set(exclude_from_all "")
  else()
    set(exclude_from_all "EXCLUDE_FROM_ALL")
  endif()

  add_executable("${TARGET}" ${exclude_from_all} ${arg_SOURCES})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} PRIVATE ${arg_LIB_DEPS} ${arg_OBJ_DEPS})

  if(arg_PKG_DEPS)
    cc_add_package_dependencies("${TARGET}"
      LIB_TYPE ${DEFAULT_LIB_TYPE}
      DEPENDS ${arg_PKG_DEPS}
    )
  endif()

  if(install)
    set(install_args
      TARGETS "${TARGET}"
      RUNTIME
    )

    if(arg_DESTINATION)
      list(APPEND install_args DESTINATION ${arg_DESTINATION})
    endif()

    install(${install_args} COMPONENT "${arg_INSTALL_COMPONENT}")
  endif()
endfunction()
