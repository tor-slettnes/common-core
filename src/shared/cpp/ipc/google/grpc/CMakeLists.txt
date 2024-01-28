## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Name of this library. This can be used as a dependency for downstream libraries/executables.
set(TARGET cc_shared_ipc_grpc)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_shared_ipc_api
  cc_shared_ipc_protobuf
)

### External package dependencies, configured via `pkg-config`.
set(PKG_DEPS
  grpc++
)

set(SOURCES
  grpc-status.c++
  grpc-basewrapper.c++
  grpc-clientwrapper.c++
  grpc-servicewrapper.c++
)


### Recepies to build the library
include(BuildLibrary)

### Include binary output dir
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

### Generate "grpc-status-names.gen"
set(GRPCSTATUSFILE   ${CMAKE_CURRENT_BINARY_DIR}/grpc-status-names.gen)
set(GRPCSTATUSSCRIPT ${CMAKE_SOURCE_DIR}/scripts/generate_grpcstatus.py)
add_custom_command(
    OUTPUT ${GRPCSTATUSFILE}
    COMMAND python3 ${GRPCSTATUSSCRIPT} --output ${GRPCSTATUSFILE} --sysroot "${BUILD_ROOT}/"
    COMMENT "Generating ${GRPCSTATUSFILE}"
    BYPRODUCTS ${GRPCSTATUSFILE})
add_custom_target(grpcstatuslist DEPENDS ${GRPCSTATUSFILE})
add_dependencies(${TARGET} grpcstatuslist)

### Define preprocessor symbol
target_compile_definitions(${TARGET} PUBLIC USE_GRPC=1)
