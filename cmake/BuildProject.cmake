##
## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for building a project
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

  message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

  ### Common utility functions
  include(pkgconf)

  ### Enable testing
  include(CTest)

  # ### Include rules to build doxygen
  # include(Doxygen)
endif()
