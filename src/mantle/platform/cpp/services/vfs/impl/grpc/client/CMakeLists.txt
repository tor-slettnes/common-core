## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET "cc_platform_vfs_grpc_client")

set(LIB_DEPS
  cc_platform_vfs_base
  cc_platform_vfs_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  vfs-grpc-provider.c++
  vfs-grpc-stream.c++
  vfs-remote-context.c++
  vfs-providers-grpc.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
