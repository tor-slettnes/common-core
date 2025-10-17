## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for building a project
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Source additional files from this folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

### Identify the Common Core root folder even if used as a submodule
cmake_path(SET CC_BUILDCONFIG_DIR NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/..")
cmake_path(SET CC_BUILDSCRIPTS_DIR "${CC_BUILDCONFIG_DIR}/scripts")

cmake_path(SET CC_PROJECT_DIR NORMALIZE "${CC_BUILDCONFIG_DIR}/..")
cmake_path(SET CC_SOURCE_DIR "${CC_PROJECT_DIR}/src")

cmake_path(SET OUTPUTS_DIR NORMALIZE "${CMAKE_BINARY_DIR}/..")

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
