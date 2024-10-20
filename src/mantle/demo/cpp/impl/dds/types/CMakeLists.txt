## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for demo types conversions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_demo_dds_types)

### Library dependencies.
set(LIB_DEPS
  cc_demo_api
  cc_demo_idl
  cc_core_io_idl
)

set(SOURCES
  translate-idl-demo.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_TYPE ${STATIC}
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
