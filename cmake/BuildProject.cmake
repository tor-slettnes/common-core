## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for building a project
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

if(NOT PROJECT_INCLUDED)
  SET(PROJECT_INCLUDED true)

  ### Source additional files from this folder
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

  ### Identify the Common Core root folder even if used as a submodule
  get_filename_component(COMMON_CORE_DIR ".." REALPATH
    BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

  ### Folder in which to find custom build scripts
  get_filename_component(BUILD_SCRIPTS_DIR "../scripts" REALPATH
    BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

  ### Assign installation folder
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (NOT $ENV{CMAKE_INSTALL_PREFIX} STREQUAL "")
      get_filename_component(_install_prefix "$ENV{CMAKE_INSTALL_PREFIX}" REALPATH
        BASE_DIR "${CMAKE_SOURCE_DIR}")

      set(CMAKE_INSTALL_PREFIX "${_install_prefix}"
        CACHE PATH "Folder in which to install build artifacts"
        FORCE)
    endif()
  endif()

  ### Common utility functions
  include(pkgconf)

  ### Enable testing
  include(CTest)

  ### Include rules to build doxygen
  include(Doxygen)

  ### Load targeted build functions
  include(BuildProto)
  include(BuildLibrary)
  include(BuildExecutable)
  include(BuildPython)
  include(BuildSettings)
  include(BuildPackage)
  include(BuildDebianService)

  #=============================================================================
  ### CPack configuration

  ### Load CPack configuration, first from this folder
  foreach(dir
      "${CMAKE_CURRENT_LIST_DIR}"
      "${COMMON_CORE_DIR}"
      "${CMAKE_SOURCE_DIR}")

    include("${dir}/CPackConfig.cmake"
      OPTIONAL
      RESULT_VARIABLE CPackConfig_FOUND)

    if(CPackConfig_FOUND)
      message(STATUS "Found ${dir}/CPackConfig.cmake")
    else()
      message(STATUS "Did not find ${dir}/CPackConfig.cmake")
    endif()
  endforeach()
  include(CPack)
endif()
