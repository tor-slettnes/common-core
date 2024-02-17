## -*- cmake -*-
#===============================================================================
## @file BuildProtodefs.cmake
## @brief CMake rules to generate/build ProtoBuf/gRPC bindings from ".proto" sources
## @author Tor Slettnes <tslettnes@picarro.com>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
##
## @code
##     ### Target name. This can be used as a downstream dependency.
##     set(TARGET picarro_YOURAPP_protodefs)
##
##     ### What kind of library we are building (STATIC|SHARED|OBJECT|...)
##     ### See: https://cmake.org/cmake/help/latest/command/add_library.html.
##     ### The default is OBJECT
##     set(LIB_TYPE OBJECT)
##
##     ### ProtoBuf dependencies from other applications on which we depend (if applicable).
##     set(PROTO_DEPS picarro_shared_protodefs)
##
##     ### Static/shared library dependencies, either from this build or provided by
##     ### the system. Only direct dependencies are needed; if you include
##     ### "picarro_shared_protodefs" in PROTO_DEPS you do not need "grpc++" or "protobuf"
##     ### here.
##     set(LIB_DEPS grpc++ protobuf)
##
##     ### Source files
##     set(SOURCES
##       YOURFILE1.proto
##       ...
##       )
##
##     ### Invoke common Protobuf rules
##     include(BuildProtodefs)
## @endcode
#===============================================================================

### CMake 3.14 required for Protobuf support
cmake_minimum_required(VERSION 3.14)

### For statements like "import DEPENDENCY.proto" to work, update the PROTOBUF_IMPORT_DIRS
### environment variable from the "source_dirs" property from each of our direct dependencies.
set(PROTOBUF_IMPORT_DIRS "${TARGET_ROOT}/usr/include")

foreach(PROTO_DEP ${PROTO_DEPS})
  get_target_property(source_dirs ${PROTO_DEP} source_dirs)
  list(APPEND PROTOBUF_IMPORT_DIRS ${source_dirs})
endforeach()

### Load ProtoBuf and gRPC support modules
find_package(Protobuf REQUIRED)
if(BUILD_GRPC)
  find_package(grpc REQUIRED)
endif()


### Rules to generate C++ files
if (BUILD_CPP)
  protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${SOURCES})
  if(BUILD_GRPC)
    grpc_generate_cpp(GRPC_SRC GRPC_HDR ${CMAKE_CURRENT_BINARY_DIR} ${SOURCES})
  endif()
endif()


### Python support
if (BUILD_PYTHON)
  protobuf_generate_python(PROTO_PY ${SOURCES})
  if (BUILD_GRPC)
    grpc_generate_python(GRPC_PY ${CMAKE_CURRENT_BINARY_DIR} ${SOURCES})
  endif()

  if(NOT PYTHON_INSTALL_DIR)
    set(PYTHON_INSTALL_DIR "share/python")
  endif()

  install(FILES ${PROTO_PY} ${GRPC_PY} DESTINATION ${PYTHON_INSTALL_DIR}/generated)
endif()

### Global ProtoBuf include folder
include_directories(${PROTOBUF_INCLUDE_DIRS})

### Unless LIB_TYPE is defined, set it to OBJECT
if (NOT LIB_TYPE)
  set(LIB_TYPE OBJECT)
endif()

### Build targets
##
### Fugly hack: Python ProtoBuf/gRPC output files listed here to force
### generation, though not actually included in the archive.
##
add_library("${TARGET}" "${LIB_TYPE}" ${PROTO_SRC} ${GRPC_SRC} ${PROTO_PY} ${GRPC_PY})

### Libraries required to link downstream consumers of this library
target_link_libraries(${TARGET} ${LIB_DEPS} ${PROTO_DEPS})

### Downstream consumers of this library need to include our generated sources
target_include_directories(${TARGET} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

### Use target property "source_dirs" to store location of shared .proto files
### for downstream consumers of this library.
set_target_properties(${TARGET} PROPERTIES
  source_dirs "${source_dirs};${CMAKE_CURRENT_SOURCE_DIR}"
)
