## -*- cmake -*-
#===============================================================================
## @file build_idl.cmake
## @brief CMake include file for building DDS intterfaces from IDL files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Locate RTI tools
include(rti-connext-dds)

### Define function go generate sources
macro(generate_sources sources example_modules)
  ### Construct extra flags to `rtiddsgen` based on options and dependencies
  set(include_flags)

  foreach (dep ${LIB_DEPS})
    get_target_property(include_dirs ${dep} INCLUDE_DIRECTORIES)
    foreach (dir ${include_dirs})
      list(APPEND include_flags -I${dir})
    endforeach()
  endforeach()

  #=============================================================================
  ### Invoke RTI DDS Generator on each IDL input file.
  foreach (src ${sources})
    ### Determine basename and C++ sources
    string(REGEX REPLACE "(.*)\\.idl$" "\\1" basename ${src})

    #===========================================================================
    ### C++ sources
    if (BUILD_CPP)
      set(cpp_sources ${basename}.cxx ${basename}Plugin.cxx)

      set(cpp_flags)
      if(GENERATE_EXAMPLE)
        list(APPEND cpp_flags "-example" ${CONNEXTDDS_ARCH})
      endif()


      # ### Include C++ examples (either Publisher/Subscriber or Client/Server)
      # if(GENERATE_EXAMPLE)
      #   foreach(example ${example_modules})
      #     set(gensrc "${basename}_${example}.cxx")
      #     set(target "${basename}-${example}")
      #     add_executable(${target} ${CMAKE_CURRENT_BINARY_DIR}/${gensrc})
      #     target_link_libraries(${target} ${TARGET})
      #     install(TARGETS ${target} RUNTIME)
      #     message(STATUS "Generating ${example} example: ${target}")
      #     list(APPEND cpp_sources ${gensrc})
      #   endforeach()
      # endif()

      ### Generate C++ language bindings
      list(APPEND GENERATED_SOURCES ${cpp_sources})
      add_custom_command(
        OUTPUT ${cpp_sources}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
        COMMAND ${RTICODEGEN}
          -update typefiles
          -replace
          -language C++11
          -alwaysUseStdVector
          -platform ${CONNEXTDDS_ARCH}
          -inputIdl ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
          # -unboundedSupport
          # -optimization 2
          -d ${CMAKE_CURRENT_BINARY_DIR}
          ${include_flags}
          ${cpp_flags}
        COMMENT "Generating C++ bindings from '${basename}.idl'"
      )
    endif()

    #===========================================================================
    ### C# sources
    if(BUILD_CSHARP)
      set(cs_sources ${basename}.cs ${basename}Plugin.cs)
      set(cs_flags)
      if(GENERATE_EXAMPLE)
        list(APPEND cs_flags "-example" net5)
      endif()

      list(APPEND GENERATED_SOURCES ${cs_sources})
      add_custom_command(
        OUTPUT ${cs_sources}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
        COMMAND ${RTICODEGEN}
          -update typefiles
          -replace
          -dotnet modern
          -language "c#"
          -inputIdl ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
          -d ${CMAKE_CURRENT_BINARY_DIR}
          ${include_flags}
          ${cs_flags}
        COMMENT "Generating C# bindings from '${basename}.idl'")
    endif()

    #===========================================================================
    ### Generate XML output, required for Python
    if(BUILD_PYTHON)
      set(xml_file ${basename}.xml)
      list(APPEND GENERATED_SOURCES ${xml_file})
      add_custom_command(
        OUTPUT ${xml_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
        COMMAND ${RTICODEGEN}
          -convertToXml
          -inputIdl ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.idl
          -d ${CMAKE_CURRENT_BINARY_DIR}
          ${extra_flags}
          ${include_flags}
        COMMENT "Generating XML from '${basename}.idl': ${xml_file}")
    endif()
  endforeach()
endmacro()

#===============================================================================
## @fn build_idl
## @brief create C++ library from .`idl` files

function(build_idl TARGET)
  set(_singleargs LIB_TYPE SCOPE GENERATE_EXAMPLE)
  set(_multiargs RECIPES SERVICES IDL_DEPS LIB_DEPS OBJ_DEPS PKG_DEPS)
  cmake_parse_arguments(arg "${_options}" "${_singleargs}" "${_multiargs}" ${ARGN})

  if(arg_LIB_TYPE)
    string(TOUPPER "${arg_LIB_TYPE}" _type)
  else()
    set(_type STATIC)
  endif()

  if(arg_SCOPE)
    string(TOUPPER "${arg_SCOPE}" _scope)
  else()
    set(_scope PUBLIC)
  endif()

  if(arg_GENERATE_EXAMPLE)
    set(GENERATE_EXAMPLE ${arg_GENERATE_EXAMPLE})
  else()
    set(GENERATE_EXAMPLE OFF)
  endif()

  set(_pkg_deps ${arg_PKG_DEPS})
  set(_sources ${arg_SOURCES})
  foreach(_dep ${arg_OBJ_DEPS})
    list(APPEND _sources $<TARGET_OBJECTS:${_dep}>)
  endforeach()

  add_library("${TARGET}" ${_type})

  target_include_directories("${TARGET}" ${_scope}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries("${TARGET}" ${_scope}
    ${arg_LIB_DEPS}
    ${arg_OBJ_DEPS}
    ${arg_PROTO_DEPS}
    RTIConnextDDS::cpp2_api
  )

  # set(CPP_SOURCES)
  # set(CS_SOURCES)
  # set(XML_OUTPUTS)
  set(GENERATED_SOURCES)

  #===============================================================================
  ### Generate sources from data types listed in RECIPES
  if (arg_RECIPES)
    generate_sources("${arg_RECIPES}" "publisher;subscriber")
  endif()

  ### Generate sources from RPC service interfaces listed in SERVICES
  if(arg_SERVICES)
    generate_sources("${arg_SERVICES}" "client;service")

    string(TOLOWER "$<CONFIG>" buildconfig)
    if("${buildconfig}" STREQUAL "debug")
      target_link_libraries("${TARGET}" ${_scope} "rticonnextmsgcpp2zd")
    else()
      target_link_libraries("${TARGET}" ${_scope} "rticonnextmsgcpp2z")
    endif()
  endif()

  if(GENERATED_SOURCES)
    target_sources("${TARGET}" ${_scope} ${GENERATED_SOURCES})
  endif()

  #===============================================================================
  ### Require C++11 features
  target_compile_features("${TARGET}" PUBLIC cxx_std_11)

  # ### Generate python outputs
  # if(BUILD_PYTHON)
  #   install(FILES ${XML_OUTPUTS} DESTINATION share/xml)
  # endif()
endfunction()


