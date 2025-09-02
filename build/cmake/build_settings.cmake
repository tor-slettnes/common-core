## -*- cmake -*-
#===============================================================================
## @file cc_add_settings.cmake
## @brief CMake include file to "build" (install) settings files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_path(SET SETTINGS_STAGING_DIR "${OUTPUTS_DIR}/settings")

### Add the above directories to the global `clean` target
set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES  ${SETTINGS_STAGING_DIR}
)

#===============================================================================
## @fn cc_add_settings
## @brief
##  Stage settings files/directories for output

function(cc_add_settings TARGET)
  set(_options
    RECURSE                # Recursively descend into provided DIRECTORIES
  )
  set(_singleargs
    DESTINATION            # Override default target folder
    WHEEL_DESTINATION      # Override target folder inside `.whl` files
    STAGING_DIR            # Override defualt staging folder
    INSTALL_COMPONENT      # CPack install component
  )
  set(_multiargs
    FILES                  # Explicit file paths
    DIRECTORIES            # Directories from which to collect matching files
    FILENAME_PATTERN       # Filename masks to collect from provided DIRECTORIES
    SETTINGS_DEPS          # Add build dependency on additional settings targets
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  ### We populate sources into a target-specific staging directory, from where
  ### they will be:
  ###  * Picked up by `cc_add_python_wheel()` and added to `pyproject.toml`
  ###  * Installed

  ### Specify root folder for staging python modules for this target
  cc_get_value_or_default(
    staging_dir
    arg_STAGING_DIR
    "${SETTINGS_STAGING_DIR}/${TARGET}")

  if(arg_FILES OR arg_DIRECTORIES)
    set(files "${arg_FILES}")
    set(directories "${arg_DIRECTORIES}")
  else()
    set(files)
    set(directories "./")
  endif()

  cc_get_value_or_default(
    filename_pattern
    arg_FILENAME_PATTERN
    "*.json;*.yaml;*.ini")

  cc_get_optional_keyword(RECURSE arg_RECURSE)
  cc_get_staging_list(
    FILES ${files}
    DIRECTORIES ${directories} ${RECURSE}
    FILENAME_PATTERN "${filename_pattern}"
    OUTPUT_DIR ${staging_dir}
    SOURCES_VARIABLE sources
    OUTPUTS_VARIABLE staged_outputs
  )

  if(NOT TARGET ${TARGET})
    add_custom_target("${TARGET}" ALL
      DEPENDS ${staged_outputs})
  endif()

  ### Populate `SOURCES` property for downstream dependents.
  ### (It's marked `PRIVATE` because a custom target cannot have INTERFACE sources;
  ### however still available by looking up the `SOURCES` property explicitly).
  target_sources(${TARGET} PRIVATE ${staged_outputs})

  if(arg_SETTINGS_DEPS)
    add_dependencies("${TARGET}" ${arg_SETTINGS_DEPS})
  endif()

  ### Set target properties for downstream targets
  ###   - `staging_dir` indicating where to find these settings files
  ###   - `data_dir` to indicate where these files should be added
  ###     to the final target by cc_add_python_wheel()/cc_add_python_executable().

  cc_get_argument_or_default(
    data_dir
    arg_WHEEL_DESTINATION
    "settings"
    "${arg_KEYWORDS_MISSING_VALUES}"
  )

  set_target_properties("${TARGET}" PROPERTIES
    staging_dir "${staging_dir}"
    data_dir "${data_dir}"
  )

  ### Add commands to perform the actual staging.
  add_custom_command(
    OUTPUT ${staged_outputs}
    COMMENT "${TARGET}: staging settings files"
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${staging_dir}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${staging_dir}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_EXPAND_LISTS
    VERBATIM
  )

  if(files)
    add_custom_command(
      OUTPUT ${staged_outputs} APPEND
      DEPENDS ${files}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy ${files} ${staging_dir}
    )
  endif()

  cc_get_ternary(glob_command arg_RECURSE "GLOB_RECURSE" "GLOB")

  foreach(dir ${directories})
    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR "${dir}"
      OUTPUT_VARIABLE abs_dir)

    cmake_path(GET abs_dir
      PARENT_PATH anchor_dir)

    list(TRANSFORM filename_pattern
      PREPEND "${abs_dir}/"
      OUTPUT_VARIABLE path_mask)

    file(${glob_command} rel_paths
      RELATIVE "${anchor_dir}"
      LIST_DIRECTORIES FALSE
      CONFIGURE_DEPENDS
      ${path_mask})

    foreach(rel_path ${rel_paths})
      add_custom_command(
        OUTPUT ${staged_outputs} APPEND
        DEPENDS ${anchor_dir}/${rel_path}
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${anchor_dir}/${rel_path} ${staging_dir}/${rel_path}
      )
    endforeach()
  endforeach()


  ### Install from staging folder, if requested.
  if(arg_INSTALL_COMPONENT)
    cc_get_value_or_default(destination
      arg_DESTINATION
      "${SETTINGS_DIR}")

    install(
      DIRECTORY "${staging_dir}/"
      DESTINATION "${destination}"
      COMPONENT "${arg_INSTALL_COMPONENT}")
  endif()
endfunction()
