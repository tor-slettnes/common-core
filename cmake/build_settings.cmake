## -*- cmake -*-
#===============================================================================
## @file cc_add_settings.cmake
## @brief CMake include file to "build" (install) settings files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(SETTINGS_DIR "share/cc/settings"
  CACHE STRING "Top-level installation directory for settings files" FORCE)

function(cc_add_settings TARGET)
  set(_options)
  set(_singleargs DESTINATION STAGING_DIR INSTALL_COMPONENT)
  set(_multiargs FILES DIRECTORIES SETTINGS_DEPS FILENAME_PATTERN)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(NOT TARGET "${TARGET}")
    add_custom_target("${TARGET}" ALL)
  endif()

  if(arg_SETTINGS_DEPS)
    add_dependencies("${TARGET}" ${arg_SETTINGS_DEPS})
  endif()

  ### We populate sources into a target-specific staging directory, from where
  ### they will be:
  ###  * Picked up by `cc_add_python_wheel()` and added to `pyproject.toml`
  ###  * Installed

  ### Specify root folder for staging python modules for this target
  if(arg_STAGING_DIR)
    cmake_path(APPEND "${CMAKE_CURRENT_BINARY_DIR}" "${arg_STAGING_DIR}"
      OUTPUT_VARIABLE staging_dir)
  else()
     set(staging_dir "${CMAKE_CURRENT_BINARY_DIR}/staging")
  endif()

  if(arg_FILES OR arg_DIRECTORIES)
    set(files "${arg_FILES}")
    set(directories "${arg_DIRECTORIES}")
  else()
    set(files)
    set(directories "./")
  endif()

  if(files)
    FILE(COPY "${files}"
      DESTINATION "${staging_dir}")
  endif()

  if(directories)
    cc_get_value_or_default(filename_pattern
      arg_FILENAME_PATTERN
      "*.json;*.yaml;*.ini")

    list(TRANSFORM filename_pattern PREPEND "PATTERN;"
      OUTPUT_VARIABLE match_expr)

    FILE(COPY "${directories}"
      DESTINATION "${staging_dir}"
      FILES_MATCHING ${match_expr})
  endif()

  ### Set target properties for downstream targets
  ###   - `staging_dir` indicating where to find these settings files
  ###   - `data_dir` to indicate where these files should be added
  ###     to the final target by cc_add_python_wheel()/cc_add_python_executable().

  set_target_properties("${TARGET}" PROPERTIES
    staging_dir "${staging_dir}"
    data_dir "settings"
  )

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
