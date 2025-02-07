## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build RapidJSON
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RAPIDJSON_HAS_STDSTRING ON CACHE BOOL "..." FORCE)
set(RAPIDJSON_BUILD_TESTS   OFF CACHE BOOL "..." FORCE)
# option(RAPIDJSON_HAS_STDSTRING "Build RapidJSON with std::string support" ON)
# option(RAPIDJSON_BUILD_TESTS   "Build RapidJSON testing via gtest" OFF)

include(FetchContent)

FetchContent_Declare(
  RapidJSON
  GIT_REPOSITORY             https://github.com/Tencent/rapidjson
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/rapidjson
)
