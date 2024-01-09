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

  # set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/out/install/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

  ### Include rules to build doxygen
  include(Doxygen)
endif()
