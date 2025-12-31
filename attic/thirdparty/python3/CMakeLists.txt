## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build embedded Python environment
## @author Tor Slettnes
#===============================================================================

include(FetchContent)

FetchContent_Declare(
  Python3
  GIT_REPOSITORY             https://github.com/python/cpython
  GIT_TAG                    v3.12.2
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/python3
  FIND_PACKAGE_ARGS COMPONENTS Development
)

FetchContent_MakeAvailable(Python3)
