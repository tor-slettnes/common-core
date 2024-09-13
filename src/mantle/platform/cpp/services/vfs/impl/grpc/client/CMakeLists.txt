## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_vfs_grpc_client")

set(LIB_DEPS
  cc_platform_vfs_base
  cc_platform_vfs_protobuf
  cc_core_messaging_grpc_client
)

set(SOURCES
  vfs-grpc-client.c++
  vfs-grpc-provider.c++
  vfs-grpc-stream.c++
  vfs-remote-context.c++
  vfs-providers-grpc.c++
)

BuildLibrary("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
