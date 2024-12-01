## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for generating docuemntation with Doxygen
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

find_package(Doxygen)
set(DOC_TARGET "doxygen")

if (DOXYGEN_FOUND)
  set(DOXYGEN_QUIET YES)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED no)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_CLASS_GRAPH NO)
  set(DOXYGEN_COLLABORATION_GRAPH YES)
  #set(DOXYGEN_CLANG_ASSISTED_PARSING YES)
  #set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
  #set(DOXYGEN_TEMPLATE_RELATIONS YES)
  set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
  set(DOXYGEN_DOT_TRANSPARENT YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT png)
  set(DOXYGEN_PYTHON_DOCSTRING NO)
  set(DOXYGEN_WARNINGS YES)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  #set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/docs/images/logo.png")
  #set(DOXYGEN_FILE_PATTERNS *.c++ *.h++ *.i++ *.md)
  set(DOXYGEN_EXCLUDE_PATTERNS
    "out"
    "venv"
    "attic"
    "thirdparty"
    "*/tests/*")

  add_custom_target(${DOC_TARGET})
endif()


#===============================================================================
## @fn cc_add_doxygen
## @brief Add a Doxygen build target

function(cc_add_doxygen TARGET)
  set(_options)
  set(_singleargs SOURCE_DIRECTORY OUTPUT_DIRECTORY EXAMPLE_PATH)
  set(_multiargs FILE_PATTERNS)
  cmake_parse_arguments(DOXYGEN "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(n 1)
  while(n LESS ARGC)
    set(varname "${ARGV${n}}")
    math(EXPR n "${n}+1")

    if(n LESS ARGC)
      set("DOXYGEN_${varname}" "${ARGV${n}}")
      math(EXPR n "${n}+1")
    else()
      message(ERROR "cc_add_doxygen() received key with no value: ${varname}")
    endif()
  endwhile()

  if(NOT DOXYGEN_SOURCE_DIRECTORY)
    set(DOXYGEN_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}")
  endif()

  if(NOT DOXYGEN_OUTPUT_DIRECTORY)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/docs/${TARGET}")
  endif()

  if(NOT DOXYGEN_EXAMPLE_PATH)
    set(DOXYGEN_EXAMPLE_PATH "${CMAKE_SOURCE_DIR}/src")
  endif()

  if(DOXYGEN_FOUND)
    file(RELATIVE_PATH relative_out "${CMAKE_SOURCE_DIR}" "${DOXYGEN_OUTPUT_DIRECTORY}")

    doxygen_add_docs("${TARGET}"
      "${DOXYGEN_SOURCE_DIRECTORY}"
      COMMENT "Generating Doxygen documentation: ${relative_out}")
    add_dependencies(${DOC_TARGET} "${TARGET}")
  endif()
endfunction()
