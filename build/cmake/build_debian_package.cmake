## -*- cmake -*-
#===============================================================================
## @brief CMake recipes for adding SystemD service unit
## @author Tor Slettnes <tor@slett.net>
#===============================================================================


#===============================================================================
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

#===============================================================================
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
    DEPENDS        # Other cpack components on which we depend.
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
    cc_get_value_or_default(license_file
      arg_LICENSE
      "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    )

    cc_install_license(
      FILE "${license_file}"
      COMPONENT ${COMPONENT}
      GROUP ${arg_GROUP}
    )

    string(TOUPPER "${COMPONENT}" upcase_component)

    set(description_lines ${arg_DESCRIPTION})
    if(arg_SUMMARY)
      list(PREPEND description_lines "${arg_SUMMARY}")
    endif()
    list(JOIN description_lines "\n" description_text)

    if(CPACK_COMPONENTS_GROUPING STREQUAL "IGNORE")
      set(component_deps "${arg_DEPENDS}")
    endif()

    cpack_add_component("${COMPONENT}"
      DESCRIPTION "${description_text}"
      GROUP ${arg_GROUP}
      DEPENDS ${component_deps}
    )

    foreach(option ${_multiargs})
      if (option MATCHES "^DEB_(.*)$")
        list(JOIN arg_${option} ", " predecessors)
        if(predecessors)
          cc_cpack_config(
            "CPACK_DEBIAN_${upcase_component}_PACKAGE_${CMAKE_MATCH_1}"
            "${predecessors}"
          )
        endif()
      endif()
    endforeach()
  endif()
endfunction()

#===============================================================================
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

    cc_cpack_set_debian_dependencies("${GROUP}" DEPENDS
      GROUPS ${arg_GROUP_DEPENDS}
      PACKAGES ${arg_DEB_DPEENDS}
    )
  endif()

endfunction()

#===============================================================================
## @fn cc_cpack_set_debian_dependencies

function(cc_cpack_set_debian_dependencies TARGET RELATIONSHIP)
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

#===============================================================================
## @fn cc_install_doc
## @brief Install `LICENESE.txt` into `share/doc/PACKAGE_NAME`

function(cc_install_doc)
  set(_options)
  set(_singleargs
    COMPONENT  # CPack components to which we're adding the doc
    GROUP      # CPack components group to which we're adding the doc
    RENAME     # Rename file (only for single file install)
  )
  set(_multiargs
    FILES      # Files we're adding to doc folder
  )
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if (arg_FILES)
    cc_get_cpack_debian_package_name(
      COMPONENT "${arg_COMPONENT}"
      GROUP "${arg_GROUP}"
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

#===============================================================================
## @fn cc_install_license
## @brief Install `LICENESE.txt` into `share/doc/PACKAGE_NAME`

function(cc_install_license)
  set(_singleargs
    FILE       # Source license file to install
    COMPONENT  # CPack components to which we're adding the doc
    GROUP      # CPack components group to which we're adding the doc
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
    GROUP ${arg_GROUP}
  )
endfunction()

#===============================================================================
## @fn cc_cpack_debian_config
## @brief Set a `CPACK_DEBIAN[_GROUPING]_*` value
##    depending on whether component-based packaging is enabled.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_cpack_debian_config VARIABLE_SUFFIX VALUE)
  set(_options APPEND)
  set(_singleargs
    COMPONENT # CPack component name, for when creating one package per component
    GROUP     # CPack component group, for when creating one package per group
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_cpack_debian_variable("${VARIABLE_SUFFIX}"
    COMPONENT "${arg_COMPONENT}"
    GROUP "${arg_GROUP}"
    OUTPUT_VARIABLE variable_name)

  cc_get_optional_keyword(APPEND arg_APPEND)
  cc_cpack_config("${variable_name}" "${VALUE}" ${APPEND})
endfunction()


#===============================================================================
## @fn cc_get_cpack_debian_variable
## @brief Obtain variable name corresponding to the provided suffix,
##    with with COMPONENT or GROUP inserted if the corresponding component-based
##    packaging is used.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_get_cpack_debian_variable VARIABLE_SUFFIX)
  set(_options)
  set(_singleargs
    PREFIX         # CPackConfig variable prefix (normally `CPACK_DEBIAN`)
    SUFFIX         # CPackConfig variable suffix
    COMPONENT      # CPack component for when creating one package per component
    GROUP          # CPack group for when creating one package per group
    OUTPUT_VARIABLE # Variable in which to assign the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_cpack_debian_grouping(
    PREFIX "CPACK_DEBIAN"
    SUFFIX "${VARIABLE_SUFFIX}"
    GLUE "_"
    COMPONENT "${arg_COMPONENT}"
    GROUP "${arg_GROUP}"
    OUTPUT_VARIABLE variable)

  string(TOUPPER "${variable}" upcase_variable)
  set("${arg_OUTPUT_VARIABLE}" "${upcase_variable}" PARENT_SCOPE)
endfunction()

#===============================================================================
## @fn cc_get_cpack_debian_pakage

function(cc_get_cpack_debian_package_name)
  set(_options)
  set(_singleargs
    COMPONENT      # CPack component for when creating one package per component
    GROUP          # CPack group for when creating one package per group
    OUTPUT_VARIABLE # Variable in which to store the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  cc_get_cpack_debian_grouping(
    COMPONENT "${arg_COMPONENT}"
    GROUP "${arg_GROUP}"
    PREFIX "${CPACK_PACKAGE_NAME}"
    GLUE "-"
    OUTPUT_VARIABLE name
  )

  set("${arg_OUTPUT_VARIABLE}" "${name}" PARENT_SCOPE)
endfunction()



#===============================================================================
## @fn cc_get_cpack_debian_grouping
## @brief
##    Obtain a string containing COMPONENT or GROUP if and only if
##    the corresponding component-based packaging is used.
##
## For an overview of component-specific CPack Debian variables, see:
## https://cmake.org/cmake/help/latest/cpack_gen/deb.html

function(cc_get_cpack_debian_grouping)
  set(_options)
  set(_singleargs
    PREFIX         # CPackConfig variable prefix
    SUFFIX         # CPackConfig variable suffix
    GLUE           # Delimiter beween prefix/package/suffix
    COMPONENT      # CPack component for when creating one package per component
    GROUP          # CPack group for when creating one package per group
    OUTPUT_VARIABLE # Variable in which to store the resulting CPackConfig variable name
  )
  set(_multiargs)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  set(parts ${arg_PREFIX})

  if(CPACK_DEB_COMPONENT_INSTALL)
    if(arg_COMPONENT AND CPACK_COMPONENTS_GROUPING STREQUAL "IGNORE")
      list(APPEND parts ${arg_COMPONENT})
    elseif(arg_GROUP AND CPACK_COMPONENTS_GROUPING STREQUAL "ONE_PER_GROUP")
      list(APPEND parts ${arg_GROUP})
    endif()
  endif()

  list(APPEND parts ${arg_SUFFIX})
  list(JOIN parts "${arg_GLUE}" variable)
  set("${arg_OUTPUT_VARIABLE}" "${variable}" PARENT_SCOPE)
endfunction()
