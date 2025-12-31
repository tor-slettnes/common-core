## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  yaml-parser.c++
  yaml-reader.c++
)

target_compile_definitions(${TARGET} PUBLIC
  BUILD_LIBYAML=1
)

cc_add_package_dependencies(${TARGET}
  DEPENDS yaml-0.1
)
