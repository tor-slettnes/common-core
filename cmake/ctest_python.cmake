## -*- cmake -*-
#===============================================================================
## @file ctest_python.cmake
## @brief CTest support for Python using pytest
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

function(cc_add_pytest)
  set(_options )
  set(_singleargs PYTHON_INTERPRETRER VENV WORKING_DIRECTORY)
  set(_multiargs ARGS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_PYTHON_INTERPRETER)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_PYTHON_INTERPRETER}"
      OUTPUT_VARIABLE python)
  elseif(arg_VENV)
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${arg_VENV}" "bin/python"
      OUTPUT_VARIABLE python)
  else()
    find_package(Python3
      COMPONENTS Interpreter
    )

    if(Python3_Interpreter_FOUND)
      set(python "${Python3_EXECUTABLE}")
    else()
      message(FATAL_ERRROR "cc_add_pytest() requires a Python interpreter")
    endif()
  endif()

  cc_get_value_or_default(workdir
    arg_WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}")

  file(GLOB_RECURSE test_scripts
    RELATIVE "${workdir}"
    LIST_DIRECTORIES FALSE
    CONFIGURE_DEPENDS         # Regenerate cache on new/deleted files
    "test_*.py")

  foreach(test_script ${test_scripts})
    cmake_path(GET test_script STEM test_name)

    add_test(
      NAME "${test_name}"
      COMMAND ${python} ${ARGS} "${test_script}"
      WORKING_DIRECTORY "${workdir}")
  endforeach()

endfunction()
