## -*- cmake -*-
#===============================================================================
## @file BuildProto.cmake
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

### Add a library
if(NOT LIB_TYPE)
  if (BUILD_CPP)
    set(LIB_TYPE STATIC)
  else()
    set(LIB_TYPE INTERFACE)
  endif()
endif()

add_library(${TARGET} ${LIB_TYPE})

### Downstream consumers of this library need to include our generated sources
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

### Libraries required to link downstream consumers of this library
target_link_libraries(${TARGET} PUBLIC ${PROTO_DEPS})

if(BUILD_PROTOBUF)
  find_package(Protobuf REQUIRED)
endif()

if(BUILD_GRPC)
  find_package(gRPC REQUIRED)
endif()


### Functions to generate language bindings
include(protogen)

### Rules to generate C++ bindings
if (BUILD_CPP)
  if (BUILD_PROTOBUF)
    # target_link_libraries(${TARGET} INTERFACE protobuf)
    protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${SOURCES}"
      DEPENDS "${PROTO_DEPS}"
    )
  endif()

  if (BUILD_GRPC)
    target_link_libraries(${TARGET} INTERFACE grpc++)

    ### The above fails to capture all required library dependencies in
    ### recent gRPC releases for Linux.  Unless we're already bulding from source,
    ### let's obtain additional package dependencies from `pkg-config`.
    if(NOT TARGET grpc++)
      find_package(PkgConfig)
      if(PkgConfig_FOUND)
        pkg_check_modules(PKG grpc++)
        if(PKG_FOUND)
          link_directories(${PKG_LIBRARY_LIBS})
          target_include_directories(${TARGET} SYSTEM INTERFACE ${PKG_INCLUDE_DIRS})
          target_link_libraries(${TARGET} INTERFACE ${PKG_LIBRARIES})
        endif()
      endif()
    endif()

    protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${SOURCES}"
      DEPENDS "${PROTO_DEPS}"
    )

  endif()
endif()


### Rules to generate Python bindings
if (BUILD_PYTHON)
  # add_custom_target(${TARGET}_py)
  # add_dependencies(${TARGET} ${TARGET}_py)

  if(NOT PYTHON_INSTALL_DIR)
    set(PYTHON_INSTALL_DIR "share/python")
  endif()
  set(_install_folder "${PYTHON_INSTALL_DIR}/generated")


  if (BUILD_PROTOBUF)
    protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      PROTOS "${SOURCES}"
      DEPENDS "${PROTO_DEPS}"
    )
    # add_dependencies(${TARGET}_py ${PROTO_PY})
    install(FILES ${PROTO_PY} DESTINATION "${_install_folder}")
  endif()

  if (BUILD_GRPC)
    #target_link_libraries(${TARGET} INTERFACE gRPC::grpc++)
    protogen_grpc_py(GRPC_PY
      TARGET "${TARGET}"
      PROTOS "${SOURCES}"
      DEPENDS "${PROTO_DEPS}"
    )
    # add_dependencies(${TARGET}_py ${GRPC_PY})
    install(FILES ${GRPC_PY} DESTINATION "${_install_folder}")
  endif()

  add_custom_target(${TARGET}_py DEPENDS ${PROTO_PY} ${GRPC_PY})
  add_dependencies(${TARGET} ${TARGET}_py)
endif()
