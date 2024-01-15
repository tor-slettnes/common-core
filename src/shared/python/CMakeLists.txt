## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake recipe to install Python components
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Create a library target to facilitate get_target_property()
add_library(cc_shared_python INTERFACE)

### Modules to be installed as programs, with execute permissionss
set(PROGRAMS
)

### Modules to be installed as supporting files, without execute permissions
set(FILES
  # container.py
  # dds_base.py
  # dds_listener.py
  grpc_base.py
  grpc_client.py
  grpc_server.py
  grpc_signalclient.py
  grpc_status.py
  jsonreader.py
  paths.py
  protobuf_custom_types.py
  protobuf_standard_types.py
  safe_invoke.py
  scalar_types.py
  settingsstore.py
  stringexpander.py
)

### Subdirectories
set(DIRECTORIES
)

include(BuildPython)
