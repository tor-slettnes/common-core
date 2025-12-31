## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for switchboard conversion routines
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_switchboard_dds_types)

### Library dependencies.
set(LIB_DEPS
  cc_switchboard_base
  cc_switchboard_idl
  cc_core_io_idl
)

set(SOURCES
  translate-idl-switchboard.c++
)

cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  PROTO_DEPS ${PROTO_DEPS}
  SOURCES ${SOURCES}
)
