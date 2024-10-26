## -*- cmake -*-
#===============================================================================
## @file cc_protogen.cmake
## @brief CMake rules to generate/build ProtoBuf/gRPC bindings from ".proto" sources
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

include(utility)                # cascade_inherited_property()

#===============================================================================
## @fn cc_protogen_common
## @brief Invoke `protoc` to generate bindings for a specified language/plugin

function(cc_protogen_common)
  set(_singleargs TARGET COMMENT GENERATOR PLUGIN OUT_DIR)
  set(_multiargs PROTOS DEPENDS SUFFIXES OUT_VARS)
  cmake_parse_arguments(arg "" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if(NOT arg_TARGET)
    message(SEND_ERROR "Error: cc_protogen() needs TARGET")
    return()

  elseif(NOT TARGET "${arg_TARGET}")
    add_custom_target("${arg_TARGET}")
  endif()

  if(arg_PLUGIN)
    set(_plugin_arg "--plugin=${arg_PLUGIN}")
  endif()

  ### Collect ProtoBuf source folders from this target and its dependencies in
  ### the target property `source_dirs`. The resulting list is then passed into
  ### the ProtoBuf compiler as include directories.

  cc_get_target_properties_recursively(
    PROPERTIES SOURCE_DIR
    TARGETS ${arg_DEPENDS}
    INITIAL_VALUE "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE include_dirs
    REMOVE_DUPLICATES
  )

  ## Append any directories supplied in the `Protobuf_IMPORT_DIRS` variable
  if(DEFINED Protobuf_IMPORT_DIRS)
    list(APPEND include_dirs ${Protobuf_IMPORT_DIRS})
  endif()

  ### Add `.proto` files supplied in the PROTOS argument
  set(_proto_files "${arg_PROTOS}")

  ## Also add any `.proto` files that were supplied as target sources
  get_target_property(_files ${arg_TARGET} SOURCES)
  foreach(_file ${_files})
    if (_file MATCHES "proto$")
      list(APPEND _proto_files ${_file})
    endif()
  endforeach()

  ## Determine output directory
  if(arg_OUT_DIR)
    file(REAL_PATH "${arg_OUT_DIR}" _outdir
      BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
    file(MAKE_DIRECTORY "${_outdir}")
  else()
    set(_outdir "${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  ## Constuct a list of language-specific files to be generated
  set(_proto_src)
  set(_outputs)
  foreach(_src ${_proto_files})
    get_filename_component(_abs ${_src} ABSOLUTE)
    list(APPEND _proto_src ${_abs})

    get_filename_component(_base ${_src} NAME_WLE)
    set(_stem ${_outdir}/${_base})
    set(_out_vars ${arg_OUT_VARS})

    foreach(_suffix ${arg_SUFFIXES})
      list(APPEND _outputs ${_stem}${_suffix})

      list(POP_FRONT _out_vars _out_var)
      list(APPEND ${_out_var} ${_stem}${_suffix})
    endforeach()
  endforeach()

  if(TARGET protoc)
    set(_protoc "$<TARGET_FILE:protoc>")
  else()
    find_program(_protoc protoc)
  endif()

  ## Finally let's do the generation.
  add_custom_command(
    OUTPUT ${_outputs}
    DEPENDS ${_proto_src} ${arg_DEPENDS}
    COMMAND ${_protoc}
    ARGS --${arg_GENERATOR}_out "${_outdir}"
       "-I$<JOIN:${include_dirs},;-I>"
       ${_plugin_arg}
       ${_proto_src}
    COMMAND_EXPAND_LISTS
    COMMENT "${arg_COMMENT}"
    VERBATIM
  )

  target_sources("${arg_TARGET}" PRIVATE ${_outputs})

  ### Promote each symbol from `OUT_VARS` to parent scope
  foreach(_out_var ${arg_OUT_VARS})
    set(${_out_var} ${${_out_var}} PARENT_SCOPE)
  endforeach()

endfunction()

#===============================================================================
## @fn cc_protogen_protobuf_cpp
## @brief Generate C++ ProtoBuf bindings

function(cc_protogen_protobuf_cpp SRCS HDRS)
  set(_singleargs TARGET)
  set(_multiargs PROTOS DEPENDS)
  cmake_parse_arguments(arg "" "${_singleargs}" "${_multiargs}" "${ARGN}")

  cc_protogen_common(
    TARGET "${arg_TARGET}"
    COMMENT "Generating C++ ProtoBuf bindings for ${arg_TARGET}"
    GENERATOR cpp
    DEPENDS "${arg_DEPENDS}"
    PROTOS "${arg_PROTOS}"
    SUFFIXES ".pb.cc" ".pb.h"
    OUT_VARS ${SRCS} ${HDRS})

  if(arg_TARGET)
    if(TARGET protobuf::libprotoc)
      target_include_directories(${arg_TARGET} PUBLIC
        "$<TARGET_PROPERTY:protobuf::libprotoc,INTERFACE_INCLUDE_DIRECTORIES>"
      )
    endif()
  endif()

  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_protogen_grpc_cpp
## @brief Generate C++ gRPC bindings

function(cc_protogen_grpc_cpp SRCS HDRS)
  set(_singleargs TARGET)
  set(_multiargs PROTOS DEPENDS)
  cmake_parse_arguments(arg "" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if(TARGET grpc_cpp_plugin)
    set(GRPC_CPP_PLUGIN "$<TARGET_FILE:grpc_cpp_plugin>")
  else()
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
  endif()

  cc_protogen_common(
    TARGET "${arg_TARGET}"
    COMMENT "Generating C++ gRPC bindings for ${arg_TARGET}"
    GENERATOR grpc
    PLUGIN "protoc-gen-grpc=${GRPC_CPP_PLUGIN}"
    DEPENDS "${arg_DEPENDS}"
    PROTOS "${arg_PROTOS}"
    SUFFIXES ".grpc.pb.cc" ".grpc.pb.h"
    OUT_VARS ${SRCS} ${HDRS}
  )

  if(arg_TARGET)
    if (TARGET grpc++)
      target_include_directories(${arg_TARGET} PUBLIC
        # ${gRPC_INCLUDE_DIRS})
        "$<TARGET_PROPERTY:grpc++,INTERFACE_INCLUDE_DIRECTORIES>"
      )
    endif()
  endif()

  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


#===============================================================================
## @fn cc_protogen_protobuf_py
## @brief Generate Python ProtoBuf bindings

function(cc_protogen_protobuf_py SRCS)
  set(_singleargs TARGET OUT_DIR)
  set(_multiargs PROTOS DEPENDS)
  cmake_parse_arguments(arg "" "${_singleargs}" "${_multiargs}" "${ARGN}")

  cc_protogen_common(
    TARGET "${arg_TARGET}"
    COMMENT "Generating Python ProtoBuf bindings for ${arg_TARGET}"
    GENERATOR python
    DEPENDS "${arg_DEPENDS}"
    PROTOS "${arg_PROTOS}"
    SUFFIXES "_pb2.py"
    OUT_DIR "${arg_OUT_DIR}"
    OUT_VARS ${SRCS})

  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_protogen_grpc_py
## @brief Generate Python gRPC bindings

function(cc_protogen_grpc_py SRCS)
  set(_singleargs TARGET OUT_DIR)
  set(_multiargs PROTOS DEPENDS)
  cmake_parse_arguments(arg "" "${_singleargs}" "${_multiargs}" "${ARGN}")

  if(TARGET grpc_python_plugin)
    set(GRPC_PYTHON_PLUGIN "$<TARGET_FILE:grpc_python_plugin>")
  else()
    find_program(GRPC_PYTHON_PLUGIN grpc_python_plugin)
  endif()

  cc_protogen_common(
    TARGET "${arg_TARGET}"
    COMMENT "Generating Python gRPC bindings for ${arg_TARGET}"
    GENERATOR grpc
    PLUGIN "protoc-gen-grpc=${GRPC_PYTHON_PLUGIN}"
    DEPENDS "${arg_DEPENDS}"
    PROTOS "${arg_PROTOS}"
    SUFFIXES "_pb2_grpc.py"
    OUT_DIR "${arg_OUT_DIR}"
    OUT_VARS ${SRCS}
  )

  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
endfunction()
