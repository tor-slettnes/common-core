## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET "cc_platform_vfs_grpc_service")

set(LIB_DEPS
  cc_platform_vfs_base
  cc_platform_vfs_protobuf
  cc_core_messaging_grpc_server
)

### Source files
set(SOURCES
  vfs-grpc-signalqueue.c++
  vfs-grpc-requesthandler.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
