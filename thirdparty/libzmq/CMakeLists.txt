## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(FetchContent)

FetchContent_Declare(
  libzmq
  GIT_REPOSITORY             https://github.com/zeromq/libzmq
  GIT_TAG                    v4.3.5
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/libzmq
  FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(libzmq)
