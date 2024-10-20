## -*- cmake -*-
#===============================================================================
## @brief Generate developer documentation from source using Doxygen
#===============================================================================

### Generate documentation
find_package(Doxygen)

if (DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_BRIEF "Common Core")
  set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out/reference")
  set(DOXYGEN_EXAMPLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/applications/example")
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_CLASS_DIAGRAMS YES)
  set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
  set(DOXYGEN_HAVE_DOT YES)
  set(DOXYGEN_CLASS_GRAPH YES)
  set(DOXYGEN_COLLABORATION_GRAPH YES)
  set(DOXYGEN_CLASS_GRAPH YES)
  set(DOXYGEN_GROUP_GRAPHS YES)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_GRAPHICAL_HIERARCHY YES)
  set(DOXYGEN_DIRECTORY_GRAPH YES)
  set(DOXYGEN_INCLUDE_GRAPH YES)
  set(DOXYGEN_INCLUDED_BY_GRAPH YES)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
  set(DOXYGEN_TEMPLATE_RELATIONS YES)
  set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
  set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 0)
  set(DOXYGEN_DOT_TRANSPARENT YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT png)
  set(DOXYGEN_EXCLUDE_PATTERNS
    "out"
    "thirdparty")

  doxygen_add_docs(doxygen
    "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
