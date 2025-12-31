## -*- cmake -*-
#===============================================================================
## @file ctest_python.cmake
## @brief CTest support for Python using pytest
## @author Tor Slettnes
#===============================================================================

#-------------------------------------------------------------------------------
# cc_add_pytests() - Add a `pytest` invocation per Python test module in the
#    specified locations.
#    Note that this is different from invoking `pytest` directly, in that
#    reports will be generated per file and not per individual test.

function(cc_add_pytests)
  set(_options)
  set(_singleargs
    PYTHON_INTERPRETER          # Explicit Python interpreter path
    VENV                        # Python `virtualenv` folder
    WORKING_DIRECTORY)          # Directory from which to run `pytest`
  set(_multiargs
    FILES                       # Python modules containing pytest invocations
    DIRECTORIES                 # Directories with Python modules
    FILENAME_PATTERN            # File mask within directories; default: `test_*.py`
    ARGS)                       # Additional arguments to `pytest`
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_find_python(
    ALLOW_SYSTEM
    ACTION "cc_add_pytests()"
    PYTHON_INTERPRETER "${arg_PYTHON_INTERPRETER}"
    VENV "${arg_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  cc_get_value_or_default(
    filename_pattern
    arg_FILENAME_PATTERN
    "test_*.py")

  cc_get_value_or_default(
    workdir
    arg_WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR})

  cc_get_staging_list(
    FILES ${arg_FILES}
    DIRECTORIES ${arg_DIRECTORIES} RECURSE
    FILENAME_PATTERN ${filename_pattern}
    SOURCES_VARIABLE paths
    CONFIGURE_DEPENDS)

  list(LENGTH paths num_paths)
  message(DEBUG "Adding ${num_paths} Python test modules")

  foreach(path ${paths})
    cmake_path(RELATIVE_PATH path
      OUTPUT_VARIABLE rel_path)

    add_test(
      NAME ${rel_path}
      COMMAND ${python} -m pytest ${ARGS} "${path}"
      WORKING_DIRECTORY "${workdir}")
  endforeach()
endfunction()


#-------------------------------------------------------------------------------
# cc_add_pytests_experimental() - Add a `pytest` invocation per individual
#    standalone test as well per class containing test methods.  This could be
#    extended further to invoke `pytest` for each test method *within* such a
#    class, to get a report comparable to invoking `pytest` directly.
#
#    This code is currently disabled, because it does not function properly on
#    the first invocation from a clean source tree.  In order to generate a list
#    of tests we have to invoke `pytest --collect-only`; however, if we are
#    using a Python `virtualenv` setup, it may not yet have been created.
#    (That happens later, in the build step).  We *could* work around this
#    by forcing a CMake cache regeneration as preliminary step for testing,
#    at the cost of increasingly complex/brittle code.

function(cc_add_pytests_experimental)
  set(_options)
  set(_singleargs
    PYTHON_INTERPRETER          # Explicit Python interpreter path
    VENV                        # Python `virtualenv` folder
    WORKING_DIRECTORY)          # Directory from which to run `pytest`
  set(_multiargs
    FILES                       # Python modules containing pytest invocations
    DIRECTORIES                 # Directories with Python modules
    FILENAME_PATTERN            # File mask within directories; default: `test_*.py`
    ARGS)                       # Additional arguments to `pytest`
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_find_python(
    ALLOW_SYSTEM
    ACTION "cc_add_pytests()"
    PYTHON_INTERPRETER "${arg_PYTHON_INTERPRETER}"
    VENV "${arg_VENV}"
    ALLOW_SYSTEM
    OUTPUT_VARIABLE python)

  cc_get_value_or_default(
    filename_pattern
    arg_FILENAME_PATTERN
    "test_*.py")

  cc_get_value_or_default(
    workdir
    arg_WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR})

  unset(paths)
  foreach(path ${arg_FILES} ${arg_DIRECTORIES})
    cmake_path(ABSOLUTE_PATH path NORMALIZE
      OUTPUT_VARIABLE abs_path)
    list(APPEND paths "${abs_path}")
  endforeach()

  execute_process(
    COMMAND ${python} -m pytest --collect-only --quiet --disable-warnings --no-header --no-summary ${paths}
    WORKING_DIRECTORY "${workdir}"
    OUTPUT_VARIABLE test_summary
    ERROR_QUIET
  )

  string(REPLACE "\n" ";" lines "${test_summary}")

  unset(tests)
  foreach(line ${lines})
    if (line MATCHES "^([^:]*)::([^:]*).*$")
      ### 1st match is test file relative to ${workdir};
      ### let's get it relative to ${CMAKE_CURRENT_SOURCE_DIR} instead.
      cmake_path(APPEND workdir "${CMAKE_MATCH_1}"
        OUTPUT_VARIABLE abs_path)
      cmake_path(RELATIVE_PATH abs_path
        OUTPUT_VARIABLE rel_path)

      ### 2nd match is `TestClass::test_function` or just `test_function`.
      ### Lets split it by `.`, as expected for the `pytest -k` argument.
      string(REPLACE "::" "." test_name "${CMAKE_MATCH_2}")
      list(APPEND tests "${rel_path}:${test_name}")
    endif()
  endforeach()

  list(REMOVE_DUPLICATES tests)

  foreach(test_id ${tests})
    string(REPLACE ":" ";" fields "${test_id}")

    list(GET fields 0 filename)
    list(GET fields 1 testname)

    cmake_path(ABSOLUTE_PATH filename
      OUTPUT_VARIABLE filepath)

    add_test(
      NAME "${test_id}"
      COMMAND ${python} -m pytest ${ARGS} -k "${testname}" "${filepath}"
      WORKING_DIRECTORY "${workdir}"
      COMMAND_EXPAND_LISTS
    )
  endforeach()

endfunction()
