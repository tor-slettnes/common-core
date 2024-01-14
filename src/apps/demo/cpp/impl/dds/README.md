Demo Application - DDS adapter
==============================

This implementation of the [C++ API](../../api/README.md) comprises four modules:

* An [RPC client module](rpc-client) which forwards invocations as RPC method invocations over the DDS message bus
* An [RPC service module](rpc-server) which serves remote RPC invocations from clients by invoking the corresponding methods within the server's process. There, this would normally be handled by the [native](../native/README.md) implementation; however, since the native API is interchangable it is technically possble that the request is further forwarded to another implementation.  This approach allows for some flexibility; for instance the RPC server module could reside inside a DDS proxy daemon that forwards requests, possibly via a different message bus.
* A [DDS publisher module](dds-publisher), which captures signals emitted within the sever process and propagates these to listeneres over the DDS message bus.
* A [DDS subscriber module](dds-subscriber) which captures publications from the server, andand re-emits these locally as signals within the client process.


Data type translations and portability
--------------------------------------

Since the above modules communcate over the DDS bus, their data exchange is defined in [Interface Defintion Language](../../../idl/README.md) files located in the [idl](../../../idl) folder.  However, by design, neither our abstract API nor our native/core implementation has any dependency on DDS, and thus do not utilize the above IDL data types.

Thus, our implementation includes a number of `encode()` and `decode()` method signatures to translate between the native and IDL data types that make up our DDS Pub/Sub and Client/Server interfaces.  These methods are located in the [types](types) folder.

(In turn, these make use of similar `encode()`/`decode()` routines from the [idl](../../../../../shared/cpp/ipc/rti/idl) folder from our shared repository).

As an application developer you may certainly choose to omit this level of data abstraction, and instead use the IDL-generated types natively.  Note however that doing so would increase your coupling to to DDS, and complicate any future efforts to choose a different communication infrastructure.
