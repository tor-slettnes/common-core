## -*- cmake -*-
#===============================================================================
## @file BuildProto.cmake
## @brief Wrapper function for building libraries from `.proto` files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(protogen)
include(pkgconf)

#===============================================================================
## @fn BuildProto_CPP

function(BuildProto_CPP TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE)
  set(_multiargs  PROTOS LIB_DEPS OBJ_DEPS PKG_DEPS PROTO_DEPS)
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

  set(_sources)
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  add_library("${TARGET}" "${_type}" ${_sources})

  target_include_directories("${TARGET}" ${_scope}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries("${TARGET}" ${_scope}
    ${arg_LIB_DEPS}
    ${arg_OBJ_DEPS}
    ${arg_PROTO_DEPS}
  )

  add_package_dependencies("${TARGET}"
    LIB_TYPE "${_type}"
    DEPENDS "${arg_PKG_DEPS}")

  if(BUILD_PROTOBUF)
    find_package(Protobuf REQUIRED)

    protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      DEPENDS "${arg_PROTO_DEPS}"
    )
  endif()

  if(BUILD_GRPC)
    find_package(gRPC REQUIRED)

    ### The above fails to capture all required library dependencies from recent
    ### gRPC releases.  Let's obtain additional package dependencies from
    ### `pkg-config`.
    add_package_dependencies("${TARGET}"
      LIB_TYPE "${_type}"
      DEPENDS grpc++)

    protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      DEPENDS "${arg_PROTO_DEPS}"
    )
  endif()

endfunction()

#===============================================================================
## @fn BuildProto_PYTHON

function(BuildProto_PYTHON TARGET)
  set(_options)
  set(_singleargs COMPONENT INSTALL_DIR PACKAGE)
  set(_multiargs PROTOS LIB_DEPS OBJ_DEPS PROTO_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if (arg_INSTALL_DIR)
    set(_install_dir "${arg_INSTALL_DIR}")
  else()
    set(_install_dir "${PYTHON_INSTALL_DIR}")
  endif()

  if(arg_PACKAGE)
    set(_install_dir "${_install_dir}/${_arg_PACKAGE}")
  elseif(PYTHON_PACKAGE)
    set(_install_dir "${_install_dir}/${PYTHON_PACKAGE}")
  endif()

  set(_install_dir "${_install_dir}/generated")

  add_custom_target(${TARGET} DEPENDS ${LIB_DEPS} ${OBJ_DEPS})

  if (BUILD_PROTOBUF)
    protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      DEPENDS "${arg_PROTO_DEPS}"
      OUT_DIR "python"
    )

    install(
      FILES ${PROTO_PY}
      DESTINATION "${_install_dir}"
      COMPONENT "${arg_COMPONENT}"
    )
  endif()

  if (BUILD_GRPC)
    protogen_grpc_py(GRPC_PY
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      DEPENDS "${arg_PROTO_DEPS}"
      OUT_DIR "python"
    )

    install(
      FILES ${GRPC_PY}
      DESTINATION "${_install_dir}"
      COMPONENT "${_component}"
    )
  endif()

endfunction()


#===============================================================================
## @fn BuildProto

function(BuildProto TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE COMPONENT PYTHON_INSTALL_DIR PYTHON_PACKAGE)
  set(_multiargs SOURCES PROTO_DEPS LIB_DEPS OBJ_DEPS PKG_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_COMPONENT)
    set(_component ${arg_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  add_library("${TARGET}" INTERFACE)

  if(BUILD_CPP)
    list(TRANSFORM arg_PROTO_DEPS APPEND "_cpp" OUTPUT_VARIABLE _proto_deps_cpp)
    BuildProto_CPP("${TARGET}_cpp"
      PROTOS "${arg_SOURCES}"
      LIB_TYPE "${arg_LIB_TYPE}"
      SCOPE "${arg_SCOPE}"
      LIB_DEPS "${arg_LIB_DEPS}"
      OBJ_DEPS "${arg_OBJ_DEPS}"
      PKG_DEPS "${arg_PKG_DEPS}"
      PROTO_DEPS "${_proto_deps_cpp}"
    )
    target_link_libraries("${TARGET}" INTERFACE "${TARGET}_cpp")
  endif()

  if (BUILD_PYTHON)
    list(TRANSFORM arg_PROTO_DEPS APPEND "_py" OUTPUT_VARIABLE _proto_deps_py)
    BuildProto_PYTHON("${TARGET}_py"
      COMPONENT "${_component}"
      INSTALL_DIR "${arg_PYTHON_INSTALL_DIR}"
      PACKAGE "${arg_PYTHON_PACKAGE}"
      PROTOS "${arg_SOURCES}"
      LIB_DEPS "${arg_LIB_DEPS}"
      OBJ_DEPS "${arg_OBJ_DEPS}"
      PROTO_DEPS "${_proto_deps_py}"
    )
  endif()
endfunction()
