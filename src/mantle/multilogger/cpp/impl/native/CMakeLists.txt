## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core components
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_multilogger_native)

set(LIB_DEPS
  cc_multilogger_base
)

if(BUILD_SQLITE3)
  list(APPEND LIB_DEPS cc_multilogger_sink_sqlite3)
endif()

set(SOURCES
  multilogger-native.c++
  multilogger-native-listener.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
