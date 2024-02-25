Shared Python modules
=====================

Directory stsructure
--------------------

* [core](core) - General-purpose modules:
  - [invocation.py](core/invocation.py) - Invoke routines with exception handling & diagnostics
  - [jsonreader.py](core/jsonreader.py) - JSON file parser with support for C-style comments (`//`)
  - [paths.py](core/paths.py) - various convenience methods to find system/install/settings paths
  - [settingsstore.py](core/settingsstore.py) - Load site-specific and factory default settings/configuration
  - [scheduler.py](core/scheduler.py) - Schedule invocations at fixed time intervals
  - [scalar_types.py](core/scalar_types.py) - Misc. utilities for enums & integers
  - [signal_slot.py](core/signal_slot.py) - Event notifications using [Signal/Slot pattern](https://en.wikipedia.org/wiki/Signals_and_slots)
  - [stringexpander.py](core/stringexpander.py) - parse string using shell-style `${}`/`$()`/`$[]` expansions

* [io](io) - I/O interfaces
 - [protobuf](io/protobuf) - Containers and methods for various [ProtoBuf](https://protobuf.dev/) types

* [messaging](messaging) - Modules for inter-process communication (messaging):
 - [base](messaging/base) - Base modules/classes for all messaging platforms
 - [grpc](messaging/grpc) - Communication modules for [gRPC](https://grpc.io/)
 - [http](messaging/http) - Wrapper for HTTP client, URL parser...
 - [rest](messaging/rest) - REST API client
 - [zmq](messaging/zmq) - Communication modules for [ZeroMQ](https://zeromq.org/)
    [basic](messaging/zmq/basic) - General purpose endpoints: publisher/subscriber, requester/responder, ...
    [protobuf](messaging/zmq/protobuf) - Communicate with ProtoBuf messages over ZMQ, including a custom  client/server RPC scheme

