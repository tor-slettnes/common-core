Shared Python modules
=====================

Modules
-------
* `protobuf_standard_types.py` -- Convenience wrappers to access ProtoBuf types provided by Google
* `protobuf_custom_types.py` -- Convenience wrappers to access our own custom types
* `grpc_base.py` -- General support for gRPC applications
* `grpc_client.py` -- Abstract wrapper class for gRPC client stubs
* `grpc_signalclient.py` -- Special-purpose client to obtain events from multiplexed `Signal` streams
* `grpc_server.py` -- Abstract wrapper class for gRPC server stubs
* `grpc_status.py` -- Methods to extract additional detail from custom gRPC status responses
* `jsonreader.py` -- Read a .json file with comments
* `paths.py` -- locate settings and other paths
* `settingsstore.py` -- obtain and merge settings from upstream and instrument-specific configuration files
* `safe_invoke.py` -- invoke arbitrary methods/calllbacks with exception handling/logging
* `scalar_types.py` -- miscellaneous data types, including `EnumValue` and alternate integer representations
* `stringexpander.py` -- expand strings with placeholders for variables, nested Python expressions, etc

