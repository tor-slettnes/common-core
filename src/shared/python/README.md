Shared Python modules
=====================

Directory stsructure
--------------------

* [core](core) - General-purpose modules:
  - [paths.py](core/paths.py) - various convenience methods to find system/install/settings paths
  - [jsonreader.py](core/jsonreader.py) - JSON file parser with support for C-style comments (`//`)
  - [settingsstore.py](core/settingsstore.py) - Load site-specific and factory default settings/configuration
  - [invocation.py](core/invocation.py) - Invoke routines with exception handling & diagnostics
  - [scheduler.py](core/scheduler.py) - Schedule invocations at fixed time intervals
  - [scalar_types.py](core/scalar_types.py) - Misc. utilities for enums & integers
  - [stringexpander.py](core/stringexpander.py) - parse string using shell-style `${}`/`$()`/`$[]` expansions

* [ipc](ipc) - Modules for inter-process communication (messaging):
 - [base](ipc/base) - Base modules/classes for all messaging platforms
 - [protobuf](ipc/protobuf) - Containers and methods for various [ProtoBuf](https://protobuf.dev/) types
 - [grpc](ipc/grpc) - Communication modules for [gRPC](https://grpc.io/)
 - [zmq](ipc/zmq) - Communication modules for [ZeroMQ](https://zeromq.org/)
    [basic](ipc/zmq/basic) - General purpose endpoints: publisher/subscriber, requester/responder, ...
    [protobuf](ipc/zmq/protobuf) - Communicate with ProtoBuf messages over ZMQ, including a custom  client/server RPC scheme
 - [dds](ipc/dds) - Communications modules for [OMG Distributed Data Service](https://www.omg.org/omg-dds-portal/) (incomplete)
