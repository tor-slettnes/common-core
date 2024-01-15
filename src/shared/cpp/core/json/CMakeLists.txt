## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

target_sources(${TARGET} PUBLIC
  jsondecoder.c++
  jsonreader.c++)

target_compile_definitions(${TARGET} PUBLIC RAPIDJSON_HAS_STDSTRING)

