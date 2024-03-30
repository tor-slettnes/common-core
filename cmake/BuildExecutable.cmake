## -*- cmake -*-
#===============================================================================
## @file BuildExecutable.cmake
## @brief BuildExecutable.cmake used for building binaries/executables
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your
## "CMakeLists.txt":
##
## @code
##     ### Name of this executable. This also becomes the name of the target.
##     set(TARGET yourexecutable)
##
##     ### Destination directory, if other than `bin`.
##     #set(DEST_DIR sbin)
##
##     ### Libraries we depend on, either from this build or provided by the system.
##     ### Only direct dependencies are needed.
##     set(LIB_DEPS YOURAPP_library)
##
##     ### Any object library dependencies that are not included in the library
##     ### dependency above.
##     set(OBJ_DEPS YOURAPP_objectlib)
##
##     ### Any custom dependencies that will be built prior to this library
##     set(PRE_DEPS ...)
##
##     ### Source files
##     set(SOURCES
##       yourfile1.c++
##       yourfile2.c++
##       ...
##       )
##
##     ## Invoke common CMake rules to build executable
##     include(BuildExecutable)
## @endcode
#===============================================================================

### Project name
project(${TARGET} LANGUAGES CXX)

### CMake π or newer required for implicit install locations
cmake_minimum_required(VERSION 3.14)

### Update sources to include files from any direct object library dependencies.
foreach (lib ${OBJ_DEPS})
  list(APPEND SOURCES $<TARGET_OBJECTS:${lib}>)
endforeach()

### Build exeutable for sources above.
add_executable(${TARGET} ${SOURCES})

### Add required library dependencies
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET} PUBLIC ${LIB_DEPS} ${OBJ_DEPS})

### Add pre-dependencies
if(PRE_DEPS)
  add_dependencies(${TARGET} ${PRE_DEPS})
endif()

### Install executable
if (DEST_DIR)
  install(
    TARGETS ${TARGET}
    RUNTIME
    DESTINATION ${DEST_DIR})
else()
  install(
    TARGETS ${TARGET}
    RUNTIME)
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
  target_link_libraries(${TARGET} PUBLIC ${PKG_LIBRARIES})
endforeach()
