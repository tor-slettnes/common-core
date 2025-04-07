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
  cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH CC_BUILDCONFIG_DIR)
  cmake_path(GET CC_BUILDCONFIG_DIR PARENT_PATH CC_PROJECT_DIR)

  cmake_path(APPEND CC_BUILDCONFIG_DIR "scripts"
    OUTPUT_VARIABLE CC_BUILDSCRIPTS_DIR)

  cmake_path(APPEND CC_PROJECT_DIR "src"
    OUTPUT_VARIABLE CC_SOURCE_DIR)

  # Set build version, date and time
  string(TIMESTAMP BUILD_TIME "%s")

  ### Merge in shared configuration in case this is a submodule within a parent project.
  include(${CC_BUILDCONFIG_DIR}/buildspec.cmake)

  ### Common utility functions
  include(utility)
  include(pkgconf)

  #-----------------------------------------------------------------------------
  ### CTest configuration
  include(ctest_python)
  include(CTest)

  #-----------------------------------------------------------------------------
  ### Load targeted build functions

  include(build_library)
  include(build_executable)
  include(build_python)
  include(build_python_venv)
  include(build_python_wheel)
  include(build_python_executable)
  include(build_proto)
  include(build_settings)
  include(build_debian_package)
  include(build_debian_service)
  include(doxygen)

  #-----------------------------------------------------------------------------
  ### CPack configuration

  include(cpack_init)
  include(CPack)

endif()
