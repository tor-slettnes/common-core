## -*- cmake -*-
#===============================================================================
## @file BuildProto.cmake
## @brief Wrapper function for building libraries from `.proto` files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(protogen)
include(pkgconf)
include(BuildPython)

#===============================================================================
## @fn BuildProto

function(BuildProto TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE INSTALL_COMPONENT PYTHON_INSTALL_DIR PYTHON_NAMESPACE)
  set(_multiargs SOURCES PROTO_DEPS LIB_DEPS OBJ_DEPS PKG_DEPS MOD_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_INSTALL_COMPONENT)
    set(_component ${arg_INSTALL_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  endif()

  if(arg_LIB_TYPE)
    set(_lib_type ${arg_LIB_TYPE})
  else()
    set(_lib_type STATIC)
  endif()

  if(NOT BUILD_CPP)
    set(_scope INTERFACE)
  elseif(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  else()
    set(_scope PUBLIC)
  endif()


  if(BUILD_CPP)
    BuildLibrary("${TARGET}"
      SCOPE    "${_scope}"
      LIB_TYPE "${arg_LIB_TYPE}"
      LIB_DEPS "${arg_LIB_DEPS}"
      OBJ_DEPS "${arg_OBJ_DEPS}" "${arg_PROTO_DEPS}"
      PKG_DEPS "${arg_PKG_DEPS}"
      MOD_DEPS "${arg_MOD_DEPS}"
    )

    BuildProto_CPP("${TARGET}"
      LIB_TYPE "${_lib_type}"
      SCOPE "${_scope}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS "${arg_SOURCES}"
    )
  endif()

  if(BUILD_PYTHON)
    if(arg_PYTHON_NAMESPACE)
      set(_namespace "${arg_PYTHON_NAMESPACE}")
    else()
      list(FIND arg_KEYWORDS_MISSING_VALUES PYTHON_NAMESPACE _found)
      if(${_found} LESS 0 AND PYTHON_NAMESPACE)
        set(_namespace "${PYTHON_NAMESPACE}")
      endif()
    endif()

    BuildProto_PYTHON("${TARGET}"
      INSTALL_COMPONENT "${_component}"
      INSTALL_DIR "${arg_PYTHON_INSTALL_DIR}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS "${arg_SOURCES}"
    )
  endif()
endfunction()


#===============================================================================
## @fn BuildProto_CPP

function(BuildProto_CPP TARGET)
  set(_options)
  set(_singleargs LIB_TYPE SCOPE)
  set(_multiargs  DEPENDS PROTOS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  target_include_directories("${TARGET}" ${arg_SCOPE}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  if(BUILD_PROTOBUF)
    find_package(Protobuf REQUIRED)

    protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

  if(BUILD_GRPC)
    find_package(gRPC REQUIRED)

    # The above fails to capture all required library dependencies from recent
    # gRPC releases.  Let's obtain additional package dependencies from
    # `pkg-config`.
    add_package_dependencies("${TARGET}"
      LIB_TYPE "${arg_LIB_TYPE}"
      DEPENDS gpr)

    protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

endfunction()

#===============================================================================
## @fn BuildProto_PYTHON

function(BuildProto_PYTHON TARGET)
  set(_options)
  set(_singleargs STAGING_DIR INSTALL_COMPONENT INSTALL_DIR NAMESPACE NAMESPACE_COMPONENT)
  set(_multiargs DEPENDS PROTOS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_NAMESPACE_COMPONENT)
    set(_namespace_component "${arg_NAMESPACE_COMPONENT}")
  else()
    set(_namespace_component "generated")
  endif()

  if(arg_STAGING_DIR)
    set(staging_dir "${arg_STAGING_DIR}")
  else()
    # set(staging_dir "${CMAKE_BINARY_DIR}/python-protos")
    set(staging_dir "${CMAKE_CURRENT_BINARY_DIR}/python")
  endif()

  ### Construct namespace for Python modules
  get_namespace(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${_namespace_component}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
    OUTPUT_VARIABLE namespace)

  get_namespace_dir(
    NAMESPACE "${namespace}"
    ROOT_DIR "${staging_dir}"
    OUTPUT_VARIABLE gen_dir)

  ### Generate Python bindings
  file(MAKE_DIRECTORY "${gen_dir}")

  if(BUILD_PROTOBUF)
    protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOS}"
      OUT_DIR "${gen_dir}"
    )
  endif()

  if(BUILD_GRPC)
    protogen_grpc_py(GRPC_PY
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOS}"
      OUT_DIR "${gen_dir}"
    )
  endif()

  if(arg_DEPENDS)
    add_dependencies("${TARGET}" ${arg_DEPENDS})
  endif()

  ### Set target properties for downstream targets
  ###   - `staging_dir` indicating where to find the generated modules
  ###   - `required_packages` to indicate that this namespace must
  ###     be explicitly imported by `BuildPythonExecutable()`.
  set_target_properties("${TARGET}" PROPERTIES
    staging_dir "${staging_dir}"
    required_packages "${namespace}"
  )

  ### Install generated Python modules if requested
  if(INSTALL_PYTHON_MODULES AND arg_INSTALL_COMPONENT)
    if(arg_INSTALL_DIR)
      set(_install_dir "${arg_INSTALL_DIR}")
    else()
      set(_install_dir "${PYTHON_INSTALL_DIR}")
    endif()

    install(DIRECTORY "${staging_dir}/"
      DESTINATION "${_install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}")
  endif()
endfunction()

