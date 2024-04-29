## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for building a project
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(NOT PROJECT_INCLUDED)
  cmake_minimum_required(VERSION 3.20)

  SET(PROJECT_INCLUDED true)

  if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()

  ### Source additional files from this folder
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

  ### Folder in which to find custom build scripts
  get_filename_component(BUILD_SCRIPTS_DIR "../scripts" REALPATH
    BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

  get_filename_component(COMMON_CORE_DIR ".." REALPATH
    BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

  # file(REAL_PATH "../scripts" BUILD_SCRIPTS_DIR
  #   BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

  ### Assign installation folder
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (NOT $ENV{CMAKE_INSTALL_PREFIX} STREQUAL "")
      get_filename_component(_install_prefix "$ENV{CMAKE_INSTALL_PREFIX}" REALPATH
        BASE_DIR "${CMAKE_SOURCE_DIR}")
      # file(REAL_PATH "$ENV{CMAKE_INSTALL_PREFIX}" _install_prefix
      #   BASE_DIRECTORY "${CMAKE_SOURCE_DIR}")
      set(CMAKE_INSTALL_PREFIX "${_install_prefix}"
        CACHE PATH "Folder in which to install build artifacts"
        FORCE)
    endif()
  endif()

  ### Common utility functions
  include(pkgconf)

  ### Enable testing
  include(CTest)

  include(BuildProto)
  include(BuildLibrary)
  include(BuildExecutable)
  include(BuildPython)
  include(BuildSettings)

  ### Enable package creation (for now just Debian packages)
  #include(BuildPackage)

  #=============================================================================
  ### CPack configuration

  ### Load CPack configuration, first from this folder
  foreach(dir
      "${CMAKE_CURRENT_LIST_DIR}"
      "${COMMON_CORE_DIR}"
      "${CMAKE_SOURCE_DIR}")

    include("${dir}/CPackConfig.cmake"
      OPTIONAL
      RESULT_VARIABLE CPACKCONFIG_FOUND)

    if(CPACKCONFIG_FOUND)
      message(STATUS "Found ${dir}/CPackConfig.cmake")
    else()
      message(STATUS "Did not find ${dir}/CPackConfig.cmake")
    endif()
  endforeach()
  include(CPack)

  ### Include rules to build doxygen
  include(Doxygen)
endif()
