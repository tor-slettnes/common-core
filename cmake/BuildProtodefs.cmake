## -*- cmake -*-
#===============================================================================
## @file BuildProtodefs.cmake
## @brief CMake rules to generate/build ProtoBuf/gRPC bindings from ".proto" sources
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
##
## @code
##     ### Target name. This can be used as a downstream dependency.
##     set(TARGET cc_YOURAPP_protodefs)
##
##     ### What kind of library we are building (STATIC|SHARED|OBJECT|...)
##     ### See: https://cmake.org/cmake/help/latest/command/add_library.html.
##     ### The default is OBJECT
##     set(LIB_TYPE OBJECT)
##
##     ### ProtoBuf dependencies from other applications on which we depend (if applicable).
##     set(PROTO_DEPS cc_shared_protodefs)
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
  find_package(gRPC REQUIRED)
endif()


### Rules to generate Python bindings
if (BUILD_PYTHON)
  protobuf_generate_python(PROTO_PY ${SOURCES})
  if (BUILD_GRPC)
    grpc_generate_python(GRPC_PY ${CMAKE_CURRENT_BINARY_DIR} ${SOURCES})
  endif()

  if(NOT PYTHON_INSTALL_DIR)
    set(PYTHON_INSTALL_DIR "share/python")
  endif()

  add_custom_target(${TARGET}_python DEPENDS ${PROTO_PY} ${GRPC_PY})
  set(PRE_DEPS ${TARGET}_python)
  install(FILES ${PROTO_PY} ${GRPC_PY} DESTINATION ${PYTHON_INSTALL_DIR}/generated)
endif()


### Rules to generate C++ bindings
if (BUILD_CPP)
  protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${SOURCES})
  if(BUILD_GRPC)
    grpc_generate_cpp(GRPC_SRC GRPC_HDR ${CMAKE_CURRENT_BINARY_DIR} ${SOURCES})

    ### Use the gRPC++ target from `FindgRPC.cmake`
    list(APPEND LIB_DEPS gRPC::grpc++)

    ### However the above fails to capture all required library dependencies in
    ### recent gRPC releases for Linux. Let's obtain additional dependencies
    ### from `pkg-config`.
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
      set(PKG_DEPS grpc++)
    endif()
  endif()

  set(SOURCES
    ${PROTO_SRC} ${GRPC_SRC}
  )

  ### Global ProtoBuf include folder
  include_directories(${PROTOBUF_INCLUDE_DIRS})

  ### Unless LIB_TYPE is defined, set it to OBJECT
  if(NOT LIB_TYPE)
    set(LIB_TYPE OBJECT)
  endif()

  include(BuildLibrary)

  ### Downstream consumers of this library need to include our generated sources
  target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  ### Libraries required to link downstream consumers of this library
  target_link_libraries(${TARGET} PUBLIC ${PROTO_DEPS})

else() # No C++ build
  set(LIB_TYPE INTERFACE)
  include(BuildLibrary)
endif()



### Use target property "source_dirs" to store location of shared .proto files
### for downstream consumers of this library.
set_target_properties(${TARGET} PROPERTIES
  source_dirs "${source_dirs};${CMAKE_CURRENT_SOURCE_DIR}"
)
