## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @description CMake rules to build Common Core components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_logger_native)

set(LIB_DEPS
  cc_logger_base
)

if(BUILD_SQLITE3)
  list(APPEND LIB_DEPS cc_logger_sink_sqlite3)
endif()

set(SOURCES
  logger-native.c++
  logger-native-listener.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
