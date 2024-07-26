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
  set(_multiargs  PROTOS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  target_include_directories("${TARGET}" ${arg_SCOPE}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  if(BUILD_PROTOBUF)
    find_package(Protobuf REQUIRED)

    protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

  if(BUILD_GRPC)
    # find_package(gRPC REQUIRED)

    # The above fails to capture all required library dependencies from recent
    # gRPC releases.  Let's obtain additional package dependencies from
    # `pkg-config`.
    add_package_dependencies("${TARGET}"
      LIB_TYPE "${arg_LIB_TYPE}"
      DEPENDS grpc++)

    protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

endfunction()

#===============================================================================
## @fn BuildProto_PYTHON

function(BuildProto_PYTHON TARGET)
  set(_options)
  set(_singleargs INSTALL_COMPONENT INSTALL_DIR PACKAGE)
  set(_multiargs PROTOS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if (arg_INSTALL_DIR)
    set(_install_dir "${arg_INSTALL_DIR}")
  else()
    set(_install_dir "${PYTHON_INSTALL_DIR}")
  endif()

  if(arg_PACKAGE)
    set(_install_dir "${_install_dir}/${arg_PACKAGE}")
  endif()

  set(_install_dir "${_install_dir}/generated")

  if (BUILD_PROTOBUF)
    protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      OUT_DIR "python"
    )

    install(
      FILES ${PROTO_PY}
      DESTINATION "${_install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}"
    )
  endif()

  if (BUILD_GRPC)
    protogen_grpc_py(GRPC_PY
      TARGET "${TARGET}"
      PROTOS "${arg_PROTOS}"
      OUT_DIR "python"
    )

    install(
      FILES ${GRPC_PY}
      DESTINATION "${_install_dir}"
      COMPONENT "${_arg_INSTALL_COMPONENT}"
    )
  endif()

endfunction()


#===============================================================================
## @fn BuildProto

function(BuildProto TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE INSTALL_COMPONENT PYTHON_INSTALL_DIR PYTHON_PACKAGE)
  set(_multiargs SOURCES LIB_DEPS OBJ_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_INSTALL_COMPONENT)
    set(_component ${arg_INSTALL_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component common)
  endif()

  if(NOT BUILD_CPP)
    set(_scope INTERFACE)
  elseif(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  else()
    set(_scope PUBLIC)
  endif()

  BuildLibrary("${TARGET}"
    SCOPE    "${_scope}"
    LIB_TYPE "${arg_LIB_TYPE}"
    LIB_DEPS "${arg_LIB_DEPS}"
    OBJ_DEPS "${arg_OBJ_DEPS}"
    PKG_DEPS "${arg_PKG_DEPS}"
    MOD_DEPS "${arg_MOD_DEPS}"
  )

  if(BUILD_CPP)
    BuildProto_CPP("${TARGET}"
      LIB_TYPE "${arg_LIB_TYPE}"
      SCOPE "${_scope}"
      PROTOS "${arg_SOURCES}"
    )
  endif()

  if (BUILD_PYTHON)
    if(arg_PYTHON_PACKAGE)
      set(_package "${arg_PACKAGE}")
    else()
      list(FIND arg_KEYWORDS_MISSING_VALUES PYTHON_PACKAGE _found)
      if(${_found} LESS 0 AND CPACK_PYTHON_PACKAGE)
        set(_package "${CPACK_PYTHON_PACKAGE}")
      endif()
    endif()

    BuildProto_PYTHON("${TARGET}"
      INSTALL_COMPONENT "${_component}"
      INSTALL_DIR "${arg_PYTHON_INSTALL_DIR}"
      PACKAGE "${_package}"
      PROTOS "${arg_SOURCES}"
    )
  endif()
endfunction()
