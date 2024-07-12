Demo Application - gRPC adapter
===============================

This implementation of the [C++ API](../../api/README.md) comprises two modules:

* A [gRPC client module](client) which implements the virtual methods from the API by forwarding to corresponding invocation requests over gRPC
* A [gRPC service module](server), which handles incoming client requests by invoking the corresponding method within the server's process space. There, this would normally be handled by the [native](../native/README.md) implementation; however, since the native API is interchangable it is technically possble that the request is further forwarded to another implementation.  This approach allows for some flexibility; for instance the gRPC server module could be used within a proxy daemon to forward requests, possibly via a different message bus.

Event propagation
-----------------

Signals emitted in the server space are streamed back to the client via the following gRPC invocation:

```protobuf
    // Stream signals from the server
    rpc watch (CC.Signal.Filter) returns (stream Signal);
```

### Mechanics

This is implemented as follows:

 * The client normally calls this method in a separate thread once it has connected to the server.
 * The server handles this call by creating a [signal queue](server/demo-grpc-signalqueue.h++) instance, from which it connects to the specified signals within its own process space.
  * As signals are emitted within the server's process, they are encoded as ProtoBuf `Signal` messages and pushed onto this queue.
  * From here they are pulled into the gRPC stream by successive blocking read requests from the client.
  * Each message retrieved by the client is then decoded, and the corresponding signal is (re-)emitted locally within its own process space.

Since the signals are defined as part of the API and thus agnostic to implementation, it is transparent to the application code whether they were emitted directly from the source ([native implementation](../native/README.md)) or via this gRPC adapter.


### `Signal` message

To make this work our service requires the above `watch()` method, as well as a `Signal` message type into which the signals are encoded and multiplexed:

```protobuf
package CC.Demo;

message Signal
{
    // Mapping type, one of: MAP_NONE, MAP_ADDITION, MAP_REMOVAL, MAP_UPDATE.
    // In the case of a MappingSignal (like `Greeting` below), indicates
    // whether this instance is added, updated, or removed.
    CC.Signal.MappingAction mapping_action = 1;

    // Mapping key for the item that was added/removed/updated.
    string key = 2;

    oneof signal {
        Greeting greeting = 8;
        TimeData time = 9;
    }
}
```

The receiver then uses the `oneof signal` selector to invoke a corresponding handler, which then decodes the payload from and emits the corresponding signal locally.



Data type translations and portability
--------------------------------------

Since the above modules communcate over gRPC, their data exchange is defined in `.proto` files located in the [protodefs](../../../protodefs) folder.  However, by design, neither our abstract API nor our native/core implementation has any dependency on gRPC, and thus do not utilize the data types defined in these files.

Thus, our implementation includes a number of `encode()` and `decode()` method signatures to translate between the native and IDL data types that make up our DDS Pub/Sub and Client/Server interfaces.  These methods are located in the [types](types) folder.

(In turn, these make use of similar `encode()`/`decode()` routines from the [idl](../../../../../shared/cpp/messaging/google/protobuf) folder from our shared repository).

As an application developer you may certainly choose to omit this level of data abstraction, and instead use the generated ProtoBuf messages natively.  Note however that doing so would increase your coupling to to gRPC, and complicate any future efforts to choose a different communication infrastructure.
