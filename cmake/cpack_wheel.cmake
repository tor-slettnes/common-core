message(STATUS "----- cpack_wheel begin --------")
message(STATUS "CPACK_TOPLEVEL_DIRECTORY = ${CPACK_TOPLEVEL_DIRECTORY}")
message(STATUS "CPACK_TEMPORARY_DIRECTORY = ${CPACK_TEMPORARY_DIRECTORY}")

file(GLOB _json_file "${CPACK_TOPLEVEL_DIRECTORY}/*.json")
message(STATUS "JSON FILE: '${_json_file}'")

file(SIZE "${_json_file}" _json_file_size)
file(READ "${_json_file}" _json_text)
message(STATUS "JSON TEXT (${_json_file_size} bytes): ${_json_text}")

# string(JSON _num_groups LENGTH "${_json_text}" "componentGroups")
# message(STATUS "Group count: ${_num_groups}")

message(STATUS "----- cpack_wheel end --------")

### Generate `pyproject.toml` file
set(PACKAGE "${CPACK_PACKAGE_NAME}")
set(DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(VERSION "${CPACK_PACKAGE_VERSION}")
set(HOMEPAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")

### Get author name and email from `${CPACK_PACKAGE_CONTACT}`
if(CPACK_PACKAGE_CONTACT)
  string(REGEX REPLACE "^([^<]*) <(.*)>" "\\1" AUTHOR_NAME "${CPACK_PACKAGE_CONTACT}")
  string(REGEX REPLACE "^([^<]*) <(.*)>" "\\2" AUTHOR_EMAIL "${CPACK_PACKAGE_CONTACT}")
endif()

### Get Python package dependencies from `${CPACK_PYTHON_DEPENDENCIES}`
list(TRANSFORM CPACK_PYTHON_DEPENDENCIES REPLACE "^(.+)$" "\"\\1\"")
list(JOIN CPACK_PYTHON_DEPENDENCIES ", " DEPENDENCIES)

### Gather a list of Python folders to include
file(GLOB _python_dirs
  RELATIVE "${CPACK_TEMPORARY_DIRECTORY}"
  "${CPACK_TEMPORARY_DIRECTORY}/*/share/python")

message(STATUS "_python_dirs=${_python_dirs}")
list(TRANSFORM _python_dirs REPLACE "^(.+)$" "\"\\1\"")
list(JOIN _python_dirs ", " INCLUDE_DIRS)

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/python/pyproject.toml.in"
  "${CPACK_TEMPORARY_DIRECTORY}/pyproject.toml"
)

find_package(Python3
  COMPONENTS Interpreter
)

if(Python3_Interpreter_FOUND)
  message(STATUS "Invoking ${Python3_EXECUTABLE} -m build --outdir ${CPACK_PACKAGE_DIRECTORY} .")

  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m build --outdir "${CPACK_PACKAGE_DIRECTORY}" "."
    WORKING_DIRECTORY "${CPACK_TEMPORARY_DIRECTORY}"
  )
else()
  message(WARNING "Python3 Interpreter was not found!")
endif()
