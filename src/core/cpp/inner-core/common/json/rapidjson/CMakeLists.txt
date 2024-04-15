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

add_package_dependencies(${TARGET}
  DEPENDS RapidJSON
)

# pkgconf(RapidJSON RapidJSON)
# if(RapidJSON_FOUND)
#   target_include_directories("${TARGET}" PUBLIC ${RapidJSON_INCLUDE_DIRS})
# else()
#   FetchContent_MakeAvailable(RapidJSON)
#   target_include_directories("${TARGET}" PUBLIC ${rapidjson_SOURCE_DIR}/include)
# endif()

#add_external_dependency("${TARGET}" RapidJSON INCLUDE_DIRS)

# if(rapidjson_SOURCE_DIR)
#   target_include_directories(${TARGET} PUBLIC ${rapidjson_SOURCE_DIR}/include)
# endif()
