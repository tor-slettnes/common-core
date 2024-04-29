## -*- cmake -*-
#===============================================================================
## @file BuildProto.cmake
## @brief Wrapper function for building libraries from `.proto` files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(protogen)
include(pkgconf)

#===============================================================================
## @fn BuildProto_library

function(BuildProto TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE COMPONENT)
  set(_multiargs SOURCES PROTO_DEPS LIB_DEPS OBJ_DEPS PKG_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_LIB_TYPE)
    string(TOUPPER "${arg_LIB_TYPE}" _type)
  else()
    set(_type STATIC)
  endif()

  if(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  else()
    set(_scope PUBLIC)
  endif()

  if(arg_COMPONENT)
    set(_component ${arg_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  set(_pkg_deps ${arg_PKG_DEPS})
  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  add_library("${TARGET}" ${_type})

  target_include_directories("${TARGET}" ${_scope}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries("${TARGET}" ${_scope}
    ${arg_LIB_DEPS}
    ${arg_OBJ_DEPS}
    ${arg_PROTO_DEPS}
  )

  if(BUILD_CPP)
    if(BUILD_PROTOBUF)
      find_package(Protobuf REQUIRED)

      protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
        TARGET "${TARGET}"
        PROTOS "${_sources}"
        DEPENDS "${arg_PROTO_DEPS}"
      )
    endif()

    if(BUILD_GRPC)
      find_package(gRPC REQUIRED)

      ### The above fails to capture all required library dependencies from recent
      ### gRPC releases.  Let's obtain additional package dependencies from
      ### `pkg-config`.
      list(APPEND _pkg_deps grpc++)

      protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
        TARGET "${TARGET}"
        PROTOS "${_sources}"
        DEPENDS "${arg_PROTO_DEPS}"
      )
    endif()

    if(_pkg_deps)
      add_package_dependencies("${TARGET}"
        LIB_TYPE ${_type}
        DEPENDS ${_pkg_deps}
      )
    endif()
  endif()

  if (BUILD_PYTHON)
    if(PYTHON_INSTALL_DIR)
      set(_install_folder "${PYTHON_INSTALL_DIR}/generated")
    else()
      set(_install_folder "share/python/generated")
    endif()

    if (BUILD_PROTOBUF)
      protogen_protobuf_py(PROTO_PY
        TARGET "${TARGET}"
        PROTOS "${_sources}"
        DEPENDS "${arg_PROTO_DEPS}"
      )
      install(FILES ${PROTO_PY}
        DESTINATION "${_install_folder}"
        COMPONENT "${_component}"
      )
    endif()

    if (BUILD_GRPC)
      protogen_grpc_py(GRPC_PY
        TARGET "${TARGET}"
        PROTOS "${_sources}"
        DEPENDS "${arg_PROTO_DEPS}"
      )
      install(FILES ${GRPC_PY}
        DESTINATION "${_install_folder}"
        COMPONENT "${_component}"
      )
    endif()

    add_custom_target(${TARGET}_py DEPENDS ${PROTO_PY} ${GRPC_PY})
    add_dependencies(${TARGET} ${TARGET}_py)
  endif()
endfunction()
