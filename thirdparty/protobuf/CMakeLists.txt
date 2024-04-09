## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(FetchContent)

FetchContent_Declare(
  Protobuf
  SYSTEM
  EXCLUDE_FROM_ALL
  GIT_REPOSITORY             https://github.com/protocolbuffers/protobuf
  GIT_TAG                    v26.1
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/protobuf
  FIND_PACKAGE_ARGS
)

set(protobuf_BUILD_TESTS OFF)
set(ABSL_PROPAGATE_CXX_STD ON)

FetchContent_MakeAvailable(Protobuf)
