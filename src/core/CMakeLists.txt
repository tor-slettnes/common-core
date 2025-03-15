## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build static/shared library
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(INSTALL_GROUP "common")

cpack_add_component_group(${INSTALL_GROUP}
  DISPLAY_NAME "Core components"
  DESCRIPTION "Common core - common components"
)

### Install "settings/" folder as data
add_subdirectory(settings)

if(BUILD_DDS)
  add_subdirectory(idl)
endif()

if(BUILD_PROTOBUF)
  add_subdirectory(proto)
endif()

if(BUILD_CPP)
  add_subdirectory(cpp)
endif()

if(BUILD_PYTHON)
  add_subdirectory(python)
endif()

### Create interface library to encapsulate the above
set(TARGET cc_core)
set(LIB_DEPS)
set(PROTO_DEPS)

if(BUILD_CPP)
  list(APPEND LIB_DEPS cc_core_inner)
endif()

if(BUILD_IDL)
  list(APPEND LIB_DEPS cc_core_idl)
endif()

if(BUILD_PROTOBUF)
  list(APPEND PROTO_DEPS cc_core_proto)
endif()

cc_add_library("${TARGET}" INTERFACE
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
)

# cpack_add_component(core-common
#   GROUP ${INSTALL_GROUP}
#   DISPLAY_NAME "Common Core - common components"
#   DESCRIPTION "Common components for Common Core application framework"
# )
