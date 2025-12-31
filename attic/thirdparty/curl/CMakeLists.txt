## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build gRPC
## @author Tor Slettnes
#===============================================================================

include(FetchContent)

FetchContent_Declare(
  CURL
  SYSTEM
  EXCLUDE_FROM_ALL
  GIT_REPOSITORY             https://github.com/curl/curl
  GIT_TAG                    curl-8_7_1
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/curl
  FIND_PACKAGE_ARGS          COMPONENTS libcurl
)

FetchContent_MakeAvailable(CURL)
