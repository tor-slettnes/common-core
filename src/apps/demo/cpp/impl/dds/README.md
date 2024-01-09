Platform Apps Example - DDS adapter
===================================

This implementation of the [C++ API](../../api/README.md) comprises four modules:

* An [RPC client module](rpc-client) which forwards invocations as RPC method invocations over the DDS message bus
* An [RPC service module](rpc-server) which serves remote RPC invocations from clients by invoking the corresponding methods within the server's process. There, this would normally be handled by the [native](../native/README.md) implementation; however, since the native API is interchangable it is technically possble that the request is further forwarded to another implementation.  For instance, if a different message bus were chosen in the future, this service could act as a proxy for legacy (DDS) clients.
* A [DDS publisher module](dds-publisher) which forwards signals emitted within the server process as publications over the DDS message bus.
* A [DDS subscriber module](dds-subscriber) which subscribes to events from the remote publisher, and re-emits these as signals within the client process.


Minimal coupling
----------------

There is no direct link-time depenedency between these four modules; instead:
- The _DDS Publisher_ and _RPC Service_ modules communicate within the server process solely via the two signals from [example/signals.h++](../../api/example/signals.h++).
- The _DDS Subscriber_ and _RPC Client_ modules are completely decoupled witin the client process.


Data type translations
----------------------

Since the above modules communcate over the DDS bus, their data exchange is defined in [Interface Defintion Language](../../../idl/README.md) files located in the [idl](../../../idl) folder.  However, by design, neither our abstract API nor our native/core implementation has any dependency on DDS, and thus do not utilize the above IDL data types.

Thus, our implementation includes a number of `encode()` and `decode()` method signatures to translate between the native and IDL data types that make up our DDS Pub/Sub and Client/Server interfaces.  These methods are located in the [types](types) folder.

(In turn, these make use of similar `encode()`/`decode()` routines from the [idl](../../../../../shared/cpp/ipc/rti/idl) folder from our shared repository).
