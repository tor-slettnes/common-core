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
  file(REAL_PATH ".." COMMON_CORE_DIR
    BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

  cmake_path(APPEND COMMON_CORE_DIR "scripts"
    OUTPUT_VARIABLE BUILD_SCRIPTS_DIR)

  # Set build version, date and time
  string(TIMESTAMP BUILD_TIME "%s")

  ### Common utility functions
  include(utility)
  include(pkgconf)

  ### Enable testing
  include(CTest)

  ### Load targeted build functions
  include(build_library)
  include(build_executable)
  include(build_python)
  include(build_python_executable)
  include(build_python_wheel)
  include(build_proto)
  include(build_settings)
  include(build_package)
  include(build_debian_service)
  include(build_docs)

  #=============================================================================
  ### CPack configuration

  include(cpack_init)
  include(CPack)
endif()
