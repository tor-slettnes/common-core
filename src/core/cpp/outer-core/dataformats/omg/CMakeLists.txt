## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for share data types
## @author Tor Slettnes
#===============================================================================

set(TARGET cc_core_io_idl)

### Library dependencies.
set(LIB_DEPS
  cc_core_idl
  cc_core_common
)

set(SOURCES
  translate-idl-variant.c++
  translate-idl-common.c++
  translate-idl-eventtypes.c++
)

### Recepies to build the library
cc_add_library("${TARGET}"
  LIB_DEPS ${LIB_DEPS}
  SOURCES ${SOURCES}
)
