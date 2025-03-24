## -*- cmake -*-
#===============================================================================
## @file cc_add_proto.cmake
## @brief Wrapper function for building libraries from `.proto` files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(protogen)
include(pkgconf)
include(build_python)

#===============================================================================
## @fn cc_add_proto

function(cc_add_proto TARGET)
  set(_options
    PYTHON_INSTALL # Install generated `.py` files even if INSTALL_COMPONENT is not set
  )
  set(_singleargs
    CPP_TARGET_SUFFIX          # Appended to CMake target for generated C++ files
    PYTHON_TARGET_SUFFIX       # Appended to CMake target for generated Python files
    LIB_TYPE                   # C++ library type (Default: STATIC)
    SCOPE                      # Target scope (Default: PUBLIC)
    PYTHON_INSTALL_COMPONENT   # Install component for generated Python files
    PYTHON_INSTALL_DIR         # Relative install folder for generated Python files
    PYTHON_NAMESPACE           # Override Python namespace (Default: cc.generated)
    PYTHON_NAMESPACE_COMPONENT # Override Python 2nd level namespace (Default: generated)
  )
  set(_multiargs
    SOURCES        # Source `.proto` files
    PROTO_DEPS     # Upstream CMake targets on which we depend
    LIB_DEPS       # Upstream libraries
    OBJ_DEPS       # Upstream CMake object libraries
    PKG_DEPS       # 3rd party package dependencies described with `pkgconf`
    MOD_DEPS       # 3rd party package dependencies described with CMake modules
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(BUILD_CPP)
    cc_get_value_or_default(cpp_suffix arg_CPP_TARGET_SUFFIX "_cpp")
    set(cpp_target "${TARGET}${cpp_suffix}")

    list(TRANSFORM arg_PROTO_DEPS
      APPEND "${cpp_suffix}"
      OUTPUT_VARIABLE proto_cpp_deps)

    cc_add_proto_cpp("${cpp_target}"
      LIB_TYPE "${arg_LIB_TYPE}"
      SCOPE "${arg_SCOPE}"
      PROTOS "${arg_SOURCES}"
      PROTO_DEPS "${proto_cpp_deps}"
      LIB_DEPS "${arg_LIB_DEPS}"
      OBJ_DEPS "${arg_OBJ_DEPS}"
      PKG_DEPS "${arg_PKG_DEPS}"
      MOD_DEPS "${arg_MOD_DEPS}"
    )
  endif()

  if(BUILD_PYTHON)
    cc_get_value_or_default(py_suffix arg_PYTHON_TARGET_SUFFIX "_py")
    set(py_target "${TARGET}${py_suffix}")

    list(TRANSFORM arg_PROTO_DEPS
      APPEND "${py_suffix}"
      OUTPUT_VARIABLE proto_py_deps)

    cc_get_optional_keyword(INSTALL arg_INSTALL)
    cc_add_proto_python("${py_target}"
      NAMESPACE "${arg_PYTHON_NAMESPACE}"
      NAMESPACE_COMPONENT "${arg_PYTHON_NAMESPACE_COMPONENT}"
      INSTALL ${INSTALL}
      INSTALL_COMPONENT "${arg_PYTHON_INSTALL_COMPONENT}"
      INSTALL_DIR "${arg_PYTHON_INSTALL_DIR}"
      DEPENDS "${proto_py_deps}"
      PROTOS "${arg_SOURCES}"
    )
  endif()
endfunction()


#===============================================================================
## @fn cc_add_proto_cpp

function(cc_add_proto_cpp TARGET)
  set(_options)
  set(_singleargs
    LIB_TYPE
    SCOPE
  )
  set(_multiargs
    PROTOS         # Source `.proto` files
    PROTO_DEPS     # Upstream CMake targets on which we depend
    LIB_DEPS       # Upstream libraries
    OBJ_DEPS       # Upstream CMake object libraries
    PKG_DEPS       # 3rd party package dependencies described with `pkgconf`
    MOD_DEPS       # 3rd party package dependencies described with CMake modules
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" scope)
  else()
    set(scope PUBLIC)
  endif()

  if(arg_LIB_TYPE)
    set(lib_type ${arg_LIB_TYPE})
  elseif(scope STREQUAL INTERFACE)
    set(lib_type INTERFACE)
  else()
    set(lib_type STATIC)
  endif()

  cc_add_library("${TARGET}"
    SCOPE    "${scope}"
    LIB_TYPE "${lib_type}"
    LIB_DEPS "${arg_LIB_DEPS}" "${arg_PROTO_DEPS}"
    OBJ_DEPS "${arg_OBJ_DEPS}"
    PKG_DEPS "${arg_PKG_DEPS}"
    MOD_DEPS "${arg_MOD_DEPS}"
  )

  target_include_directories("${TARGET}" ${scope}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  if(arg_PROTO_DEPS)
    add_dependencies("${TARGET}" ${arg_PROTO_DEPS})
  endif()

  if(BUILD_PROTOBUF)
    find_package(Protobuf REQUIRED)

    cc_protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

  if(BUILD_GRPC)
    find_package(gRPC REQUIRED)

    # The above fails to capture all required library dependencies from recent
    # gRPC releases.  Let's obtain additional package dependencies from
    # `pkg-config`.
    cc_add_package_dependencies("${TARGET}"
      LIB_TYPE "${lib_type}"
      DEPENDS gpr)

    cc_protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS "${arg_PROTOS}"
    )
  endif()

endfunction()

#===============================================================================
## @fn cc_add_proto_python

function(cc_add_proto_python TARGET)
  set(_options
    INSTALL # Install Python modules even if `INSTALL_COMPONENT` is not specified
  )
  set(_singleargs
    STAGING_DIR              # Override staging directory
    INSTALL_COMPONENT        # Install component for generated Python files
    INSTALL_DIR              # Relative install folder for generated files
    NAMESPACE                # Override namespace (Default: cc.generated)
    NAMESPACE_COMPONENT      # Override 2nd level namespace (Default: generated)
  )
  set(_multiargs
    DEPENDS                  # Upstream CMake targets on which we depend
    PROTOS                   # Source `.proto` files
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_value_or_default(
    namespace_component
    arg_NAMESPACE_COMPONENT
    "generated")

  cc_get_value_or_default(
    staging_dir
    arg_STAGING_DIR
    "${PYTHON_STAGING_ROOT}/${TARGET}")

  ### Construct namespace for Python modules
  cc_get_namespace(
    NAMESPACE "${arg_NAMESPACE}"
    NAMESPACE_COMPONENT "${namespace_component}"
    MISSING_VALUES "${arg_KEYWORDS_MISSING_VALUES}"
    OUTPUT_VARIABLE namespace)

  cc_get_namespace_dir(
    NAMESPACE "${namespace}"
    ROOT_DIR "${staging_dir}"
    OUTPUT_VARIABLE gen_dir)

  if(BUILD_PROTOBUF OR BUILD_GRPC)
    ### We include this in the 'ALL` target iff we expect to install it.
    ### In other cases (e.g. if including this target in a Python wheel), this
    ### should be an explicit dependency for one or more downstream targets.
    cc_get_optional_keyword(ALL arg_INSTALL_COMPONENT)
    add_custom_target("${TARGET}" ${ALL})

    ### Generate Python bindings
    file(MAKE_DIRECTORY "${gen_dir}")
  endif()

  if(BUILD_PROTOBUF)
    cc_protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOS}"
      OUT_DIR "${gen_dir}"
    )
  endif()

  if(BUILD_GRPC)
    cc_protogen_grpc_py(GRPC_PY
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
  ###   - `collect_submodules` to indicate that this namespace must
  ###     be explicitly imported by `cc_add_python_executable()`.
  set_target_properties("${TARGET}" PROPERTIES
    staging_dir "${staging_dir}"
    collect_submodules "${namespace}"
  )

  ### Install generated Python modules if requested
  if(INSTALL_PYTHON_MODULES AND arg_INSTALL_COMPONENT)
    cc_get_value_or_default(
      install_dir
      arg_INSTALL_DIR
      "${PYTHON_INSTALL_DIR}")

    install(DIRECTORY "${staging_dir}/"
      DESTINATION "${install_dir}"
      COMPONENT "${install_component}")
  endif()
endfunction()

