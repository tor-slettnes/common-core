Common Core Apps Example - C++ implementations
===============================================

Two different C++ implementations are included here, illustrating different
building blocks within the Common Core codebase.  Each approach implements the
same abstract API, defined in [example/api.h++](../api/example/api.h++):

1. A [C++ native library](native) that directly implements the
   underlying functionality within a single process. Notifications are emitted
   directly via the signals, [as described in the API](../api/README.md).

2. A [DDS adapter library](dds), comprising a client/server pair where
   the client implements the same C++ API by forwarding invocations as RPC calls
   to the server. Signals emitted in the server space are captured and forwarded
   via Pub/Sub to the client, where they are then re-emitted.

#### Interchangability

Since these two implementations derive from the same abstract API, they are
interchangable from the perspective of the client/application.

In fact, neither the abstract API nor the *native* flavor has any RTI/DDS
dependencies; it is perfectly conceivable to one day replace it with something
else (like gRPC/ProtoBuf) without impacting the core implementation.
