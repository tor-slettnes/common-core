## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief Build IDL encoder/decoder methods for demo types conversions
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

set(TARGET cc_demo_dds_types)

### Library type.  We use static linking for convenience.
set(LIB_TYPE STATIC)

### Library dependencies.
set(LIB_DEPS
  cc_demo_api
  cc_demo_idl
  cc_shared_ipc_idl
)

set(SOURCES
  translate-idl-demo.c++
)

### Recepies to build the library
include(BuildLibrary)
