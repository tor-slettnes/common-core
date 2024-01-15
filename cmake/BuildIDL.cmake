## -*- cmake -*-
#===============================================================================
## @file BuildIDL.cmake
## @brief CMake include file for building DDS intterfaces from IDL files
## @author Tor Slettnes <tor@slett.net>
##
## To use this file, copy, uncomment, and modify the following in your "CMakeLists.txt":
##
## @code
##     ### Name of this library. This can be used as a downstream dependency
##     set(TARGET someservice_idl)
##
##     ### Static/shared library dependencies, either from this build or provided by
##     ### the system. Only direct dependencies are needed.
##     set(LIB_DEPS shared_idl ...)
##
##     ### Sources containing IDL data types. These should NOT contain any
##     ### `interface` declarations; for that, use the SOURCES variable, below.
##     set(RECIPES
##       myapp-types.idl
##       ...
##       )
##
##     ### Sources containing DDS service interface.
##     ### (an `interface` block with a `@DDSService` or `@service` decarator).
##     ### This will add an exertnal dependency on the appropriate RTI messaging
##     ### library (depending on whether the build type is Debug or Release)
##     set(SERVICES
##       myapp-service.idl)
##       ...
##       )
##
##     ## Invoke common CMake rules
##     include(BuildInterfaces)
## @endcode
#===============================================================================


### Locate RTI tools
include(rti-connext-dds)

### Unless LIB_TYPE is defined, set it to STATIC
if (NOT LIB_TYPE)
  set(LIB_TYPE STATIC)
endif()

# set(CPP_SOURCES)
# set(CS_SOURCES)
# set(XML_OUTPUTS)
set(GENERATED_SOURCES)

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
### Generate sources from data types listed in RECIPES
if(RECIPES)
  generate_sources("${RECIPES}" "publisher;subscriber")
endif()


### Generate sources from RPC service interfaces listed in SERVICES
if(SERVICES)
  generate_sources("${SERVICES}" "client;service")

  string(TOLOWER "$<CONFIG>" buildconfig)
  if("${buildconfig}" STREQUAL "debug")
    list(APPEND LIB_DEPS "rticonnextmsgcpp2zd")
  else()
    list(APPEND LIB_DEPS "rticonnextmsgcpp2z")
  endif()
endif()


#===============================================================================
### Build library from sources above.
add_library("${TARGET}" ${LIB_TYPE} ${GENERATED_SOURCES})

#===============================================================================
### Add dependency for consumers.
target_include_directories(${TARGET} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${TARGET}
  PUBLIC ${LIB_DEPS}
  RTIConnextDDS::cpp2_api)

#===============================================================================
### Require C++11 features
target_compile_features(${TARGET} PUBLIC cxx_std_11)

### Generate python outputs
if(BUILD_PYTHON)
  install(FILES ${XML_OUTPUTS} DESTINATION share/xml)
endif()

