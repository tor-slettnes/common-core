## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to fetch and build gRPC
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(FetchContent)

FetchContent_Declare(
  gRPC
  SYSTEM
  EXCLUDE_FROM_ALL
  GIT_REPOSITORY             https://github.com/grpc/grpc
  GIT_TAG                    v1.48.4
  # GIT_TAG                    b521d4a3177fa556df749365cc31232d06ba437a # v1.48.4
  GIT_SHALLOW                ON
  GIT_REMOTE_UPDATE_STRATEGY REBASE
  SOURCE_DIR                 ${FETCHCONTENT_SOURCE_DIR}/grpc
  FIND_PACKAGE_ARGS
)

set(protobuf_BUILD_TESTS OFF)
set(ABSL_PROPAGATE_CXX_STD ON)

FetchContent_MakeAvailable(gRPC)
