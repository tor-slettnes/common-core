##
## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for building a project
#===============================================================================

if(NOT PROJECT_INCLUDED)
  SET(PROJECT_INCLUDED true)

  if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  endif()

  ### Source additional files from this folder
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

  ### Folder in which to find custom build scripts
  file(REAL_PATH "../scripts" BUILD_SCRIPTS_DIR
    BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

  ### Assign installation folder
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (NOT $ENV{CMAKE_INSTALL_PREFIX} STREQUAL "")
      file(REAL_PATH "$ENV{CMAKE_INSTALL_PREFIX}" _install_prefix
        BASE_DIRECTORY "${CMAKE_SOURCE_DIR}")
      set(CMAKE_INSTALL_PREFIX "${_install_prefix}"
        CACHE PATH "Folder in which to install build artifacts"
        FORCE)
    endif()
  endif()

  message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

  ### Enable testing
  include(CTest)

  ### Include rules to build doxygen
  include(Doxygen)
endif()
