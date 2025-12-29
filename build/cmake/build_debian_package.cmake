## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

cmake_path(SET DEBIAN_TEMPLATE_DIR
  NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../debian")

cmake_path(SET DEBIAN_CONTROL_STAGING_DIR
  NORMALIZE "${CMAKE_BINARY_DIR}/deb")

### Add the above directory to the global `clean` target
set_property(
  DIRECTORY "${CMAKE_BINARY_DIR}"
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${DEBIAN_CONTROL_STAGING_DIR}
)

#-------------------------------------------------------------------------------
## @fn cc_get_debian_package_version
## @brief Obtain Debian package version

function(cc_cpack_get_debian_version OUTPUT_VARIABLE)
  foreach(var CPACK_DEBIAN_PACKAGE_VERSION CPACK_PACKAGE_VERSION PROJECT_VERSION)
    if(${var})
      string(JOIN "-" package_version ${${var}} ${CPACK_DEBIAN_PACKAGE_RELEASE})
      break()
    endif()
  endforeach()
  set("${OUTPUT_VARIABLE}" "${package_version}" PARENT_SCOPE)
endfunction()



#-------------------------------------------------------------------------------
## @fn cc_cpack_config
## @brief Append a setting to `cc_cpack_config.cmake`, to be consumed by `cpack`.

function(cc_cpack_config VARIABLE VALUE)
  set(_options
    APPEND        # Add to existing value via `list(APPEND)` rather than `set()`
  )
  set(_singleargs)
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(REPLACE "\"" "\\\"" _value "${VALUE}")
  string(TOUPPER "${VARIABLE}" _variable)

  if(arg_APPEND)
    set(_command "list(APPEND ${_variable} \"${_value}\")")
  else()
    set(_command "set(${_variable} \"${_value}\")")
  endif()

  file(APPEND "${CMAKE_BINARY_DIR}/CPackConfig.cmake" "${_command}\n")
endfunction()

#-------------------------------------------------------------------------------
## @fn cc_cpack_debian_config
## @brief Set a `CPACK_DEBIAN[_GROUPING]_*` value
##    depending on whether component-based packaging is enabled.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html


function(cc_cpack_debian_config VARIABLE_SUFFIX VALUE)
  set(_options
    APPEND                      # Append value to the resulting CPack variable
  )
  set(_singleargs
    COMPONENT                   # CPack component name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_cpack_get_debian_grouping(
    PREFIX "CPACK_DEBIAN"
    COMPONENT "${arg_COMPONENT}"
    SUFFIX "${VARIABLE_SUFFIX}"
    GLUE "_"
    OUTPUT_VARIABLE variable)

  cc_get_optional_keyword(APPEND arg_APPEND)
  cc_cpack_config("${variable}" "${VALUE}" ${APPEND})
 endfunction()



#-------------------------------------------------------------------------------
## @fn cc_cpack_add_debian_component
## @brief
##   Wrapper for `cmake_add_component()` with added dependency options.

function(cc_cpack_add_debian_component COMPONENT)
  set(_options)
  set(_singleargs
    SUMMARY        # One-line summary (first line of package description)
    GROUP          # Component group in which this component is added, if any
    LICENSE        # License file, installed as `copyright`.
  )
  set(_multiargs
    DESCRIPTION    # Detailed description. Multiple arguments are added as separate lines.
    DEPENDS        # Other cpack components on which we depend
    RECOMMENDS     # Other cpack components that are recommended
    SUGGESTS       # Other cpack components that are suggested
    DEB_PREDEPENDS # Debian "Pre-Depends:" control value
    DEB_DEPENDS    # Debian "Depends:" control value
    DEB_RECOMMENDS # Debian "Recommends:" control value
    DEB_SUGGESTS   # Debian "Suggests:" control value
    DEB_BREAKS     # Debian "Breaks:" control value
    DEB_CONFLICTS  # Debian "Conflicts:" control value
    DEB_REPLACES   # Debian "Replaces:" control value
    DEB_PROVIDES   # Debian "Provides:" control value
    DEB_ENHANCES   # Debian "Enhances:" control value
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(COMPONENT)
    set(ENV{COMPONENT_${COMPONENT}_GROUP} "${arg_GROUP}")

    cc_get_value_or_default(license_file
      arg_LICENSE
      "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    )

    cc_install_license(
      FILE "${license_file}"
      COMPONENT ${COMPONENT}
    )

    set(description_lines ${arg_DESCRIPTION})
    if(arg_SUMMARY)
      list(PREPEND description_lines "${arg_SUMMARY}")
    endif()
    list(JOIN description_lines "\n" description_text)


    if(arg_GROUP AND CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
      set(package ${arg_GROUP})
    else()
      set(package ${COMPONENT})
      set(component_deps "${arg_DEPENDS}")
    endif()

    cpack_add_component("${COMPONENT}"
      DESCRIPTION "${description_text}"
      GROUP ${arg_GROUP}
      DEPENDS ${arg_DEPENDS}
    )

    cc_cpack_get_debian_grouping(
      PREFIX "CPACK_DEBIAN"
      COMPONENT "${package}"
      SUFFIX "PACKAGE"
      GLUE "_"
      OUTPUT_VARIABLE cpack_var_base)

    cc_cpack_get_debian_version(package_version)

    foreach(option RECOMMENDS SUGGESTS)
      if(arg_${option})
        set(cpack_option "${cpack_var_base}_${option}")

        list(TRANSFORM arg_${option}
          PREPEND "${CPACK_PACKAGE_NAME}-"
          OUTPUT_VARIABLE predecessor_list)

        list(TRANSFORM predecessor_list
          APPEND " (= ${package_version})")

        list(PREPEND arg_DEB_${option} ${predecessor_list})
      endif()
    endforeach()

    foreach(option ${_multiargs})
      if (option MATCHES "^DEB_(.*)$")
        if (arg_${option})
          set(cpack_option "${cpack_var_base}_${CMAKE_MATCH_1}")
          list(JOIN arg_${option} ", " predecessors)
          cc_cpack_config("${cpack_option}" "${predecessors}")
        endif()
      endif()
    endforeach()
  endif()
endfunction()

#-------------------------------------------------------------------------------
## @fn cc_cpack_add_group
## @brief
##   Wrapper for `cmake_add_component_group()` with added dependency options.

function(cc_cpack_add_group GROUP)
  set(_options)
  set(_singleargs
    SUMMARY                    # One-line summary to describe this package group
  )
  set(_multiargs
    GROUP_DEPENDS              # Other cpack component groups on which we depend
    DEB_DEPENDS                # Other (full) package names on which we depend
    DESCRIPTION                # Detailed description of this package group
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(GROUP AND CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
    set(description_lines ${arg_DESCRIPTION})
    if(arg_SUMMARY)
      list(PREPEND description_lines "${arg_SUMMARY}")
    endif()
    list(JOIN description_lines "\n" description_text)

    cpack_add_component_group("${GROUP}"
      DESCRIPTION "${description_text}"
    )

    ### CMake bug/inconsistency: the above `DESCRIPTION` argument sets
    ### `CPACK_COMPONENT_GROUP_${GROUP}_DESCRIPTION`, however the `cpack`
    ### command uses `CPACK_COMPONENT_${GROUP}_DESCRIPTION` to populate the
    ### `Description:` line of the Debian control file.  Let's set that latter
    ### one explicitly.

    if(description_text)
      cc_cpack_config(CPACK_COMPONENT_${GROUP}_DESCRIPTION "${description_text}")
    endif()

    cc_cpack_set_debian_predecessors("${GROUP}" DEPENDS
      GROUPS ${arg_GROUP_DEPENDS}
      PACKAGES ${arg_DEB_DPEENDS}
    )
  endif()

endfunction()


#-------------------------------------------------------------------------------
## @fn cc_cpack_get_debian_grouping
## @brief
##    Obtain a string containing COMPONENT or its group if and only if the
##    corresponding component-based packaging is used.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_cpack_get_debian_grouping)
  set(_options)
  set(_singleargs
    PREFIX         # CPackConfig variable prefix
    SUFFIX         # CPackConfig variable suffix
    GLUE           # Delimiter beween prefix/package/suffix
    COMPONENT      # CPack component for when creating one package per component
    OUTPUT_VARIABLE # Variable in which to store the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(parts ${arg_PREFIX})

  if(CPACK_DEB_COMPONENT_INSTALL)
    if((CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
        AND DEFINED ENV{COMPONENT_${arg_COMPONENT}_GROUP})
      list(APPEND parts $ENV{COMPONENT_${arg_COMPONENT}_GROUP})
    elseif(arg_COMPONENT)
      list(APPEND parts ${arg_COMPONENT})
    endif()
  endif()

  list(APPEND parts ${arg_SUFFIX})
  list(JOIN parts "${arg_GLUE}" variable)
  set("${arg_OUTPUT_VARIABLE}" "${variable}" PARENT_SCOPE)
endfunction()


#-------------------------------------------------------------------------------
## @fn cc_get_debian_cpack_pakage_name

function(cc_cpack_get_debian_package_name)
  set(_options)
  set(_singleargs
    COMPONENT      # CPack component for when creating one package per component
    OUTPUT_VARIABLE # Variable in which to store the resulting package name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_cpack_get_debian_grouping(
    PREFIX "${CPACK_PACKAGE_NAME}"
    COMPONENT "${arg_COMPONENT}"
    GLUE "-"
    OUTPUT_VARIABLE name
  )

  set("${arg_OUTPUT_VARIABLE}" "${name}" PARENT_SCOPE)
endfunction()


#-------------------------------------------------------------------------------
## @fn cc_cpack_set_debian_predecessors

function(cc_cpack_set_debian_predecessors TARGET RELATIONSHIP)
  set(_options)
  set(_singleargs)
  set(_multiargs
    COMPONENTS # CPack components on which TARGET will have a relationship
    GROUPS     # CPack component groups on which TARGET will have a relationship
    PACKAGES   # Full package names on which TARGET will have a relationship
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  string(TOUPPER "${TARGET}" upcase_target)
  cc_get_value_or_default(relationship RELATIONSHIP DEPENDS)

  set(component_deps ${arg_COMPONENTS} ${arg_GROUPS})

  list(TRANSFORM component_deps
    PREPEND "${CPACK_PACKAGE_NAME}-"
    OUTPUT_VARIABLE predecessors
  )

  list(APPEND predecessors ${arg_PACKAGES})
  list(JOIN predecessors ", " value)

  if(predecessors)
    cc_cpack_config(
      "CPACK_DEBIAN_${upcase_target}_PACKAGE_${relationship}"
      "${value}"
    )
  endif()

endfunction()

#-------------------------------------------------------------------------------
## @fn cc_install_doc
## @brief Install `LICENESE.txt` into `share/doc/PACKAGE_NAME`

function(cc_install_doc)
  set(_options)
  set(_singleargs
    COMPONENT  # CPack components to which we're adding the doc
    RENAME     # Rename file (only for single file install)
  )
  set(_multiargs
    FILES      # Files we're adding to doc folder
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if (arg_FILES)
    cc_cpack_get_debian_package_name(
      COMPONENT "${arg_COMPONENT}"
      OUTPUT_VARIABLE deb_name
    )

    install(
      FILES ${arg_FILES}
      DESTINATION share/doc/${deb_name}
      RENAME ${arg_RENAME}
      COMPONENT ${arg_COMPONENT}
    )
  endif()
endfunction()

#-------------------------------------------------------------------------------
## @fn cc_install_license
## @brief Install `LICENESE.txt` into `share/doc/PACKAGE_NAME`

function(cc_install_license)
  set(_singleargs
    FILE       # Source license file to install
    COMPONENT  # CPack components to which we're adding the doc
  )
  cmake_parse_arguments(arg "" "${_singleargs}" "" ${ARGN})

  cc_get_value_or_default(license_file
    arg_FILE
    ${CMAKE_SOURCE_DIR}/LICENSE.txt
  )

  cc_install_doc(
    FILES ${license_file}
    RENAME copyright
    COMPONENT ${arg_COMPONENT}
  )
endfunction()



#-------------------------------------------------------------------------------
## @fn cc_add_debian_control_script
## @brief
##    Add a script to be invoked during Debian package installation or removal.
##
## We install this script in `share/PACKAGE_NAME/PHASE.d`, along with a
## main control script (e.g. `postinst`,`prerm` etc) to invoke `run-parts`
## on that folder.  We do this because there may be more than one script
## to invoke for a given package, for instance to install a Python wheel in
## a virtual environment and also to add it as a `systemd` service.

function(cc_add_debian_control_script)
  set(_options)
  set(_singleargs
    COMPONENT                 # CPack component to which this script belongs
    PHASE                     # One of: `preinst`, `postinst`, `prerm`, `postrm`
    TEMPLATE                  # Input template file (normally ending in `.in`)
    OUTPUT_FILE               # Script name within parts folder
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_COMPONENT AND arg_PHASE AND arg_TEMPLATE)
    cc_cpack_get_debian_package_name(
      COMPONENT "${arg_COMPONENT}"
      OUTPUT_VARIABLE PACKAGE_NAME)

    cmake_path(APPEND DEBIAN_CONTROL_STAGING_DIR ${PACKAGE_NAME} ${arg_PHASE}.d
      OUTPUT_VARIABLE staging_dir)

    if(arg_OUTPUT_FILE)
      set(script_name "${arg_OUTPUT_FILE}")
    else()
      cmake_path(GET arg_TEMPLATE STEM LAST_ONLY script_name)
    endif()

    file(MAKE_DIRECTORY "${staging_dir}")

    cmake_path(ABSOLUTE_PATH arg_TEMPLATE
      BASE_DIRECTORY "${DEBIAN_TEMPLATE_DIR}"
      OUTPUT_VARIABLE template_path)

    configure_file("${template_path}" "${staging_dir}/${script_name}" @ONLY)

    cmake_path(SET destination "share/${PACKAGE_NAME}/${arg_PHASE}.d")

    install(PROGRAMS "${staging_dir}/${script_name}"
      COMPONENT "${arg_COMPONENT}"
      DESTINATION "${destination}")

    set(control_var "controls_${PACKAGE_NAME}")
    set(controls "$ENV{${control_var}}")
    list(FIND controls "${arg_PHASE}" found_index)
    if(found_index LESS 0)
      list(APPEND controls "${arg_PHASE}")
      set(ENV{${control_var}} "${controls}")

      set(PHASE "${arg_PHASE}")
      cmake_path(ABSOLUTE_PATH destination
        BASE_DIRECTORY "${INSTALL_ROOT}"
        OUTPUT_VARIABLE PARTS_DIRECTORY)

      cmake_path(APPEND staging_dir "${arg_PHASE}" OUTPUT_VARIABLE control_script)
      configure_file("${DEBIAN_TEMPLATE_DIR}/run-hooks.in" "${control_script}" @ONLY)
      cc_cpack_debian_config(PACKAGE_CONTROL_EXTRA "${control_script}"
        COMPONENT "${arg_COMPONENT}"
        APPEND)
    endif()
  endif()
endfunction()

