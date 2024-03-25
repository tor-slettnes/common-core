## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PRIVATE
  reader.c++
  writer.c++
)

target_compile_definitions(${TARGET} PUBLIC
  RAPIDJSON_HAS_STDSTRING
  BUILD_RAPIDJSON=1
)
