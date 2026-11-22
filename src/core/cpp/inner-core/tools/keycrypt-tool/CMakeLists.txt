## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build VFS gRPC client tool
## @author Tor Slettnes
#===============================================================================

set(TOOL_NAME keycrypt-tool)
set(TARGET ${EXEC_NAME_PREFIX}${TOOL_NAME})

set(LIB_DEPS
  cc_core_platform
)

set(SOURCES
  main.c++
  options.c++
  )

cc_add_executable("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
