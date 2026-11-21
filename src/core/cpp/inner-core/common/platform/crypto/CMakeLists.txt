## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build static/shared library
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_core_crypto)

set(LIB_DEPS
  cc_core_common
)

set(MOD_DEPS
  OpenSSL
)

set(SOURCES
  crypto-secret.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  MOD_DEPS ${MOD_DEPS}
  SOURCES ${SOURCES}
)

