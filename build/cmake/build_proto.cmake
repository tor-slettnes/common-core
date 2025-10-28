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
    PYTHON_NAMESPACE           # Override default Python installation namespace
  )
  set(_multiargs
    SOURCES           # Source `.proto` files
    PROTOBUF_SOURCES  # `.proto` files containing only ProtoBuf types
    GRPC_SOURCES      # `.proto` files containing only gRPC service definitons
    PROTO_DEPS        # Source  CMake targets on which we depend
    IMPORT_DIRS       # Additional directories to include in search path
    LIB_DEPS          # Upstream libraries
    OBJ_DEPS          # Upstream CMake object libraries
    PKG_DEPS          # 3rd party package dependencies described with `pkgconf`
    MOD_DEPS          # 3rd party package dependencies described with CMake modules
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(BUILD_PROTOBUF)
    find_package(Protobuf REQUIRED)
  endif()

  if(BUILD_GRPC)
    find_package(gRPC REQUIRED)
  endif()

  if(arg_SOURCES)
    list(APPEND arg_PROTOBUF_SOURCES "${SOURCES}")
    list(APPEND arg_GRPC_SOURCES "${SOURCES}")
  endif()

  if(BUILD_CPP)
    cc_get_value_or_default(cpp_suffix arg_CPP_TARGET_SUFFIX "_cpp")
    set(cpp_target "${TARGET}${cpp_suffix}")

    list(TRANSFORM arg_PROTO_DEPS
      APPEND "${cpp_suffix}"
      OUTPUT_VARIABLE proto_cpp_deps)

    cc_add_proto_cpp("${cpp_target}"
      LIB_TYPE "${arg_LIB_TYPE}"
      SCOPE "${arg_SCOPE}"
      PROTOBUF_SOURCES "${arg_PROTOBUF_SOURCES}"
      GRPC_SOURCES "${arg_GRPC_SOURCES}"
      IMPORT_DIRS "${arg_IMPORT_DIRS}"
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

    cc_get_optional_keyword(INSTALL arg_PYTHON_INSTALL)

    cc_get_argument_or_default(
      namespace                       # OUTPUT_VARIABLE
      arg_PYTHON_NAMESPACE            # VARIABLE
      "${PYTHON_GENERATED_NAMESPACE}" # DEFAULT
      "${arg_KEWORDS_MISSING_VALUES}" # MISSING_LIST
    )

    cc_add_proto_python("${py_target}"
      NAMESPACE "${namespace}"
      INSTALL ${INSTALL}
      INSTALL_COMPONENT "${arg_PYTHON_INSTALL_COMPONENT}"
      INSTALL_DIR "${arg_PYTHON_INSTALL_DIR}"
      DEPENDS "${proto_py_deps}"
      PROTOBUF_SOURCES "${arg_PROTOBUF_SOURCES}"
      GRPC_SOURCES "${arg_GRPC_SOURCES}"
      IMPORT_DIRS "${arg_IMPORT_DIRS}"
    )
  endif()
endfunction()


#===============================================================================
## @fn cc_add_proto_cpp

function(cc_add_proto_cpp TARGET)
  set(_options)
  set(_singleargs
    LIB_TYPE         # C++ library type (Default: STATIC)
    SCOPE            # Target scope (Default: PUBLIC)
  )
  set(_multiargs
    PROTOBUF_SOURCES # `.proto` files containing ProtoBuf types
    GRPC_SOURCES     # `.proto` files containing gRPC service definitons
    IMPORT_DIRS      # Additional directories to include in search path
    PROTO_DEPS       # Upstream CMake targets on which we depend
    LIB_DEPS         # Upstream libraries
    OBJ_DEPS         # Upstream CMake object libraries
    PKG_DEPS         # 3rd party package dependencies described with `pkgconf`
    MOD_DEPS         # 3rd party package dependencies described with CMake modules
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

  if (arg_PROTO_DEPS)
    # We manually add `PROTO_DEPS` to to make this target visible to downstream
    # targets via `cc_get_target_properties_recursively()`, which in turn uses
    # the `MANUALLY_ADDED_DEPENDENCIES` property to find this and other upstream
    # protobuf targets.
    add_dependencies(${TARGET} ${arg_PROTO_DEPS})
  endif()

  if(arg_GRPC_SOURCES)
    list(APPEND arg_PROTOBUF_SOURCES "${arg_GRPC_SOURCES}")
    list(REMOVE_DUPLICATES arg_PROTOBUF_SOURCES)
  endif()

  if(BUILD_PROTOBUF AND arg_PROTOBUF_SOURCES)
    cc_protogen_protobuf_cpp(PROTO_CPP_SRCS PROTO_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS ${arg_PROTOBUF_SOURCES}
      IMPORT_DIRS ${arg_IMPORT_DIRS}
    )

  endif()

  if(BUILD_GRPC AND arg_GRPC_SOURCES)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

    # The above fails to capture all required library dependencies from recent
    # gRPC releases.  Let's obtain additional package dependencies from
    # `pkg-config`.
    cc_add_package_dependencies("${TARGET}"
      LIB_TYPE "${lib_type}"
      DEPENDS gpr)

    cc_protogen_grpc_cpp(GRPC_CPP_SRCS GRPC_CPP_HDRS
      TARGET "${TARGET}"
      DEPENDS "${arg_PROTO_DEPS}"
      PROTOS "${arg_GRPC_SOURCES}"
      IMPORT_DIRS ${arg_IMPORT_DIRS}
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
    STAGING_DIR        # Override staging directory
    INSTALL_COMPONENT  # Install component for generated Python files
    INSTALL_DIR        # Relative install folder for generated files
    NAMESPACE          # Namespace in which to install modules
  )
  set(_multiargs
    DEPENDS            # Upstream CMake targets on which we depend
    PROTOBUF_SOURCES   # `.proto` files containing ProtoBuf types
    GRPC_SOURCES       # `.proto` files containing gRPC service definitons
    IMPORT_DIRS        # Additional directories to include in search path
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_value_or_default(
    staging_dir
    arg_STAGING_DIR
    "${PYTHON_STAGING_ROOT}/${TARGET}"
  )

  cc_get_namespace_dir(
    NAMESPACE "${arg_NAMESPACE}"
    ROOT_DIR "${staging_dir}"
    OUTPUT_VARIABLE gen_dir
  )

  if(BUILD_PROTOBUF OR BUILD_GRPC)
    ### We include this in the 'ALL` target iff we expect to install it.
    ### In other cases (e.g. if including this target in a Python wheel), this
    ### should be an explicit dependency for one or more downstream targets.
    cc_get_optional_keyword(ALL arg_INSTALL_COMPONENT)
    add_custom_target("${TARGET}" ${ALL})

    if(arg_DEPENDS)
      add_dependencies("${TARGET}" ${arg_DEPENDS})
    endif()

    ### Generate Python bindings
    file(MAKE_DIRECTORY "${gen_dir}")
  endif()

  if(BUILD_PROTOBUF AND arg_PROTOBUF_SOURCES)
    find_package(Protobuf REQUIRED)

    cc_protogen_protobuf_py(PROTO_PY
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_PROTOBUF_SOURCES}"
      OUT_DIR "${gen_dir}"
      IMPORT_DIRS ${arg_IMPORT_DIRS}
    )
  endif()

  if(BUILD_GRPC AND arg_GRPC_SOURCES)
    find_program(GRPC_PYTHON_PLUGIN grpc_python_plugin)

    cc_protogen_grpc_py(GRPC_PY
      TARGET "${TARGET}"
      DEPENDS "${arg_DEPENDS}"
      PROTOS "${arg_GRPC_SOURCES}"
      OUT_DIR "${gen_dir}"
      IMPORT_DIRS ${arg_IMPORT_DIRS}
    )
  endif()

  ### Set target properties for downstream targets
  ###   - `staging_dir` indicating where to find the generated modules
  ###   - `collect_submodules` to indicate that this namespace must
  ###     be explicitly imported by `cc_add_python_executable()`.
  set_target_properties("${TARGET}" PROPERTIES
    staging_dir "${staging_dir}"
    collect_submodules "${arg_NAMESPACE}"
  )

  ### Install generated Python modules if requested
  if(INSTALL_PYTHON_MODULES AND arg_INSTALL_COMPONENT)
    cc_get_value_or_default(
      install_dir
      arg_INSTALL_DIR
      "${PYTHON_INSTALL_DIR}")

    install(DIRECTORY "${staging_dir}/"
      DESTINATION "${install_dir}"
      COMPONENT "${arg_INSTALL_COMPONENT}")
  endif()
endfunction()
