## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes
#===============================================================================

cmake_minimum_required(VERSION 3.20)

### Obtain build configuration
include(${CMAKE_CURRENT_LIST_DIR}/build/buildspec.cmake)

message(STATUS
  "Building: project \"${PROJECT}\", "
  "product \"${PRODUCT}\", "
  "version ${VERSION}, "
  "build #${BUILD_NUMBER}")

project("${PROJECT}"
  VERSION "${VERSION}"
  DESCRIPTION "${DESCRIPTION}"
  HOMEPAGE_URL "${HOME_PAGE}"
  LANGUAGES CXX
)

### Invoke common project settings
include(${CMAKE_CURRENT_LIST_DIR}/build/cmake/build_project.cmake)

### Pull in CMake files to build the actual sources
#add_subdirectory(external)
add_subdirectory(src)
