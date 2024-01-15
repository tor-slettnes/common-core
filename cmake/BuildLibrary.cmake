## -*- cmake -*-
#===============================================================================
## @file BuildLibrary.cmake
## @brief CMake include file for building libraries
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
##
## @code
##     ### Name of this library. This can be used as a downstream dependency
##     set(TARGET YOURAPP_LIBRARY)
##
##     ### What kind of library we are building (STATIC|SHARED|OBJECT|...);
##     ### see: https://cmake.org/cmake/help/latest/command/add_library.html.
##     ### The default is STATIC
##     set(LIB_TYPE STATIC)
##
##     ### Static/shared library dependencies, either from this build or provided by
##     ### the system. Only direct dependencies are needed.
##     set(LIB_DEPS cc_shared ...)
##
##     ### Any OBJECT library dependencies that are not included in the library
##     ### dependency above. In particular, we include any direct *_protodefs
##     ### dependency here (since we built that as an OBJECT library).
##     set(OBJ_DEPS YOURAPP_objectlib)
##
##     ### Source files
##     set(SOURCES
##       yourfile1.c++
##       yourfile2.c++
##       ...
##       )
##
##     ## Invoke common CMake rules
##     include(BuildLibrary)
## @endcode
#===============================================================================

### CMake π or newer required for implicit install locations
cmake_minimum_required(VERSION 3.14)

### Update sources to include files from any direct object library dependencies.
foreach (lib ${OBJ_DEPS})
  list(APPEND SOURCES $<TARGET_OBJECTS:${lib}>)
endforeach()

### Unless LIB_TYPE is defined, set it to STATIC
if (NOT LIB_TYPE)
  if(SOURCES)
    set(LIB_TYPE STATIC)
  else()
    set(LIB_TYPE INTERFACE)
  endif()
endif()

### Build library from sources above.
add_library("${TARGET}" ${LIB_TYPE} ${SOURCES})

### Add dependency for consumers.
if("${LIB_TYPE}" STREQUAL "INTERFACE")
  message(NOTICE "Creating INTERFACE library ${TARGET} depending on ${LIB_DEPS}")
  target_include_directories(${TARGET} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} INTERFACE ${LIB_DEPS} ${OBJ_DEPS})
else()
  target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} PUBLIC ${LIB_DEPS} ${OBJ_DEPS})
endif()


### Add required package dependencies
find_package(PkgConfig)
foreach (pkg ${PKG_DEPS})
  pkg_check_modules(PKG ${pkg})
  if (NOT PKG_FOUND)
    message(FATAL_ERROR
      "Unable to find '${pkg}', which is required to build '${TARGET}'. "
      "Please install the required developer library on your system "
      "(try to search your package manager for '${pkg}-dev').")
  endif()

  link_directories(${PKG_LIBRARY_DIRS})
  target_include_directories(${TARGET} SYSTEM PUBLIC ${PKG_INCLUDE_DIRS})
  target_link_libraries(${TARGET} PRIVATE ${PKG_LIBRARIES})
endforeach()
