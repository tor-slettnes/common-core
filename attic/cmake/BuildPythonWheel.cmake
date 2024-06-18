## -*- cmake -*-
#===============================================================================
## @file BuildPythonWheel.cmake
## @brief CMake include file to "build" (install) Python modules
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(DEFAULT_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/python/pyproject.toml.in")

function(BuildPythonWheel TARGET)
  set(_options)
  set(_singleargs PACKAGE TEMPLATE MODULE COMPONENT VERSION DESCRIPTION DESTINATION PKG_DIR)
  set(_multiargs CONTENTS PYTHON_DEPS PROTO_DEPS PKG_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### Ensure our package name does not contain any dashes
  string(REPLACE "-" "_" PACKAGE "${PACKAGE}")

  ### Obtain parameters from provided arguments or their respective defaults.
  if(arg_PACKAGE)
    set(PACKAGE "${arg_PACKAGE}")
  else()
    set(PACKAGE "${PROJECT_NAME}-${TARGET}")
  endif()

  if(arg_TEMPLATE)
    set(_template "${arg_TEMPLATE}")
  else()
    set(_template "${DEFAULT_TEMPLATE}")
  endif()

  if(arg_PKG_DIR)
    set(_outdir "${arg_PKG_DIR}")
  else()
    set(_outdir "${CMAKE_SOURCE_DIR}/out/packages/python-wheels")
  endif()

  if(arg_DESTINATION)
    set(_destination "${arg_DESTINATION}")
  else()
    set(_destination "share")
  endif()

  if(arg_PKG_DEPS)
    list(JOIN arg_PKG_DEPS "\", \"" _deps)
    string(REPLACE "-" "_" _deps "${_deps}")
    set(DEPENDENCIES "\"${_deps}\"")
  else()
    set(DEPENDENCIES)
  endif()

  if(arg_CONTENTS)
    list(JOIN arg_CONTENTS "\", \"" _contents)
    set(CONTENT_SPEC "packages = [\"${_contents}\"]")
  else()
    set(CONTENT_SPEC)
  endif()

  if(arg_DESCRIPTION)
    set(DESCRIPTION "${arg_DESCRIPTION}")
  else()
    set(DESCRIPTION "${PROJECT_DESCRIPTION}")
  endif()

  if(arg_VERSION)
    set(VERSION "${arg_VERSION}")
  else()
    set(VERSION "${PROJECT_VERSION}")
  endif()

  string(REPLACE "." "/" _target_dir "${arg_MODULE}")

  ### Collect folders that we want to include in the distribution,
  ### starting with the source directory
  set(_force_include
    "\"${CMAKE_CURRENT_SOURCE_DIR}\" = \"${_target_dir}/\""
  )

  ### Add any generated output files (e.g. from `configure_file()`)
  if(IS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
    list(APPEND _force_include
      "\"${CMAKE_CURRENT_BINARY_DIR}/generated\" = \"${_target_dir}/\""
    )
  endif()

  ### Walk through source directories from our dependencies
  foreach(_dep ${arg_PYTHON_DEPS})
    get_target_property(_source_dir "${_dep}" SOURCE_DIR)
    list(APPEND _force_include "\"${_source_dir}\" = \"${_target_dir}/\"")

    get_target_property(_binary_dir "${_dep}" BINARY_DIR)
    if(IS_DIRECTORY "${_binary_dir}/generated")
      list(APPEND _force_include "\"${_binary_dir}/generated\" = \"${_target_dir}/\"")
    endif()
  endforeach()

  ### Finally, add generated ProtoBuf Python bindings from `${PROTO_DEPS}` targets.
  ### These were built using `BuildProto()`, and are written to the `python` subfolder
  ### within their respective binary output directories.
  foreach(_dep ${arg_PROTO_DEPS})
    get_target_property(_binary_dir "${_dep}" BINARY_DIR)
    list(APPEND _force_include "\"${_binary_dir}/python/\" = \"${_target_dir}/generated/\"")
  endforeach()

  ### Construct our `FORCE_INCLUDE` value for Hatchling
  list(JOIN _force_include "\n" FORCE_INCLUDE)


  ### We have now gathered enough information to generate `pyproject.toml`,
  ### and then invoke the Python builder.
  configure_file("${_template}" "pyproject.toml")

  add_custom_target("${TARGET}"
    ALL
    COMMAND python3 -m build --outdir "${_outdir}" .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Building Python wheel"
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  if(PROTO_DEPS)
    add_dependencies("${TARGET}" "${PROTO_DEPS}")
  endif()


  ### Lastly, let's install this wheel.
  if(arg_COMPONENT)
    set(_component ${arg_COMPONENT})
  elseif(CPACK_CURRENT_COMPONENT)
    set(_component ${CPACK_CURRENT_COMPONENT})
  else()
    set(_component "${PACKAGE}")
  endif()

  install(
    DIRECTORY "${_outdir}"
    COMPONENT "${_component}"
    DESTINATION "${_destination}"
    FILES_MATCHING PATTERN "${PACKAGE}-${VERSION}-*.whl"
  )

endfunction()
