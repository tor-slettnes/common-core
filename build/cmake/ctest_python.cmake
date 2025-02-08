## -*- cmake -*-
#===============================================================================
## @file ctest_python.cmake
## @brief CTest support for Python using pytest
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(cc_add_pytests TARGET)
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
    ACTION "cc_add_pytests(${TARGET})"
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
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR ${path}
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
