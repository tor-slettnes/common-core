Common Core Apps Example - C++ implementations
==============================================

Two different C++ implementations are included here, illustrating different building blocks within the Common Core codebase.  Each approach implements the same abstract API, defined in [example/api.h++](../api/example/api.h++):

1. A [C++ native library](native) that directly implements the underlying functionality within a single process. Notifications are emitted directly via the signals, [as described in the API](../api/README.md).

2. A [gRPC adapter library](grpc/README.md), comprising a client/server pair where the client implements the same C++ API by forwarding invocations as RPC calls to the server. Signals emitted in the server space are captured in a [queue](grpc/server/demo-grpc-signalqueue.h++), and from there streamed back to the client where they are read and then re-emitted locally as signals.

3. A [DDS adapter library](dds/README.md), similarly comprising a client/server pair. Similar to gRPC, a dedicated [RPC client](dds/rpc-client) implements the API by forwarding requests to a corresponding [RPC server](dds/rpc-server) where it is fulfilled natively. However, in this case signals are captured in the server and propagated to the client via a [DDS Publisher](dds/dds-publisher); the client receives these and re-emits signals locally within a [DDS Subscriber](dds/dds-subscriber). module.

#### Interchangability

Since these two implementations derive from the same abstract API, they are interchangable from the perspective of the client/application.

In fact, neither the abstract API nor the *native* flavor has any gRPC or RTI/DDS dependencies; it is perfectly conceivable to replace one with the other, or something else altogether.
