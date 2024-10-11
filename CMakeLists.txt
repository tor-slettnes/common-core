## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake build recipe for Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_minimum_required(VERSION 3.15)

### Obtain build configuration
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/local.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/local.cmake)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/defaults.cmake)

project("${PROJECT}"
  VERSION "${VERSION}"
  DESCRIPTION "${DESCRIPTION}"
  HOMEPAGE_URL "${HOME_PAGE}"
  LANGUAGES CXX
)

message(STATUS
  "Building project \"${PROJECT_NAME}\" version ${PROJECT_VERSION}, build #${BUILD_NUMBER}")

### Invoke common project settings
include(${CMAKE_CURRENT_LIST_DIR}/cmake/BuildProject.cmake)


### Pull in CMake files to build the actual sources
add_subdirectory(thirdparty)
add_subdirectory(src)
#add_subdirectory(tests)
