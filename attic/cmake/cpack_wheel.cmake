#===============================================================================
## @file cpack_wheel.cmake
## @author Tor Slettnes <tor@slett.net>
## @brief CPack generator script for Python Wheel output
#===============================================================================

function(find_include_dirs SOURCE_DIR TARGET_DIR OUT_VAR)
  file(GLOB _include_dirs
    RELATIVE "${CPACK_TEMPORARY_DIRECTORY}"
    "${CPACK_TEMPORARY_DIRECTORY}/*${CPACK_PACKAGING_INSTALL_PREFIX}/${SOURCE_DIR}"
  )

  list(TRANSFORM _include_dirs REPLACE "^(.+)$" "\"\\1\" = \"${TARGET_DIR}\"")
  list(JOIN _include_dirs "\n" _include_string)
  set(${OUT_VAR} "${_include_string}" PARENT_SCOPE)
endfunction()

### Prepare data for `pyproject.toml` file
set(PACKAGE "${CPACK_PACKAGE_NAME}")
set(DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(VERSION "${CPACK_PACKAGE_VERSION}-${CPACK_BUILD_NUMBER}")
set(HOMEPAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")

### Get author name and email from `${CPACK_PACKAGE_CONTACT}`
if(CPACK_PACKAGE_CONTACT)
  string(REGEX MATCH "^([^<]+) <(.*)>" _match "${CPACK_PACKAGE_CONTACT}")
  set(AUTHOR_NAME "${CMAKE_MATCH_1}")
  set(AUTHOR_EMAIL "${CMAKE_MATCH_2}")
endif()

### Get Python package dependencies from `${CPACK_PYTHON_DEPENDENCIES}`
# list(TRANSFORM CPACK_PYTHON_DEPENDENCIES REPLACE "^(.+)$" "\"\\1\"")
# list(JOIN CPACK_PYTHON_DEPENDENCIES ", " DEPENDENCIES)
SET(DEPENDENCIES "${CPACK_PYTHON_DEPENDENCIES}")

### Gather a list of Python folders to include
find_include_dirs("${CPACK_PYTHON_INSTALL_DIR}" "/" PYTHON_INCLUDE)
find_include_dirs("${CPACK_SETTINGS_DIR}" "settings/" SETTINGS_INCLUDE)

### Create the `pyproject.toml` file for the Python build backend
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/python/pyproject.toml.in"
  "${CPACK_TEMPORARY_DIRECTORY}/pyproject.toml"
)

### Invoke the Python build
find_package(Python3
  COMPONENTS Interpreter
)

if(Python3_Interpreter_FOUND)
  message(STATUS "Invoking ${Python3_EXECUTABLE} -m build --outdir '${CPACK_PACKAGE_DIRECTORY}' .")

  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m build --wheel --outdir "${CPACK_PACKAGE_DIRECTORY}" "."
    WORKING_DIRECTORY "${CPACK_TEMPORARY_DIRECTORY}"
  )
else()
  message(WARNING "Python3 Interpreter was not found!")
endif()
